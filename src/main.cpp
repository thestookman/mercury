#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <cstdio>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace mercury {
    // Mercury v1 starter: FPS/frame-time diagnostics HUD.
    // This measures game-update timing. It does not unlock FPS or alter physics.
    constexpr float kHudUpdateInterval = 0.20f;
    constexpr std::size_t kHistoryLength = 48;

    struct Runtime {
        CCLabelBMFont* label = nullptr;
        float updateAccumulator = 0.f;
        unsigned int frameCount = 0;
        double smoothedFrameMs = 16.6667;
        bool hudVisible = true;
        std::vector<float> frameHistory;
    };

    static Runtime* g_runtime = nullptr;

    static std::string makeHudText(double fps, double frameMs) {
        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), "MERCURY  |  %.0f FPS  |  %.2f ms", fps, frameMs);
        return buffer;
    }
}

class $modify(MercuryPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        if (mercury::g_runtime) {
            delete mercury::g_runtime;
            mercury::g_runtime = nullptr;
        }

        auto* runtime = new mercury::Runtime();
        mercury::g_runtime = runtime;
        runtime->label = CCLabelBMFont::create("MERCURY  |  measuring...", "bigFont.fnt");

        if (runtime->label) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            runtime->label->setAnchorPoint({0.f, 1.f});
            runtime->label->setPosition({8.f, winSize.height - 8.f});
            runtime->label->setScale(0.38f);
            runtime->label->setZOrder(9999);
            runtime->label->setID("mercury-performance-hud");
            this->addChild(runtime->label);
        }
        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);
        auto* runtime = mercury::g_runtime;
        if (!runtime || !runtime->label) return;

        // dt is game-update timing, not a GPU-only frame-time measurement.
        if (dt > 0.f && dt < 1.f) {
            const double currentMs = static_cast<double>(dt) * 1000.0;
            runtime->smoothedFrameMs = runtime->smoothedFrameMs * 0.85 + currentMs * 0.15;
            runtime->frameHistory.push_back(static_cast<float>(currentMs));
            if (runtime->frameHistory.size() > mercury::kHistoryLength)
                runtime->frameHistory.erase(runtime->frameHistory.begin());
        }

        runtime->updateAccumulator += dt;
        runtime->frameCount++;
        if (runtime->updateAccumulator < mercury::kHudUpdateInterval) return;

        const double elapsed = runtime->updateAccumulator;
        const double fps = elapsed > 0.0 ? static_cast<double>(runtime->frameCount) / elapsed : 0.0;
        runtime->updateAccumulator = 0.f;
        runtime->frameCount = 0;
        runtime->label->setString(mercury::makeHudText(fps, runtime->smoothedFrameMs).c_str());
        runtime->label->setVisible(runtime->hudVisible);
    }

    void onExit() {
        if (mercury::g_runtime) {
            delete mercury::g_runtime;
            mercury::g_runtime = nullptr;
        }
        PlayLayer::onExit();
    }
};
