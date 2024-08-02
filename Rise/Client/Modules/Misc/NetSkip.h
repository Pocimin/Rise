#pragma once

class NetSkip : public Module
{
public:
    float delay = 500.f;

    NetSkip(int keybind = Keys::NONE, bool enabled = false) :
        Module("Net Skip", "Misc", "create fake lag spikes that makes it look like ur ping is high", keybind, enabled)
    {
        addEnum("Mode", "The mode of netskip", { "Silent" }, &mode);
        addSlider("Ms", "Delay to wait before resending packets (ms)", &delay, 0.f, 2000);
    }

    int mode = 0;

    void onEvent(ActorBaseTickEvent* event) {
        if (TimeUtils::hasTimeElapsed("NetSkip", delay, true)) {
            Global::shouldLagTicks = false;
        }
        else {
            Global::shouldLagTicks = true;
        }
        /*static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() < delay) {
            Global::shouldLagTicks = true;
        }
        else {
            Global::shouldLagTicks = false;
            lastTime = std::chrono::high_resolution_clock::now();
        }*/
    }

    void onDisabled() override {
        Global::shouldLagTicks = false;
    }
};