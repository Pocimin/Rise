#pragma once

class AirJump : public Module
{
public:
    AirJump(int keybind = 7, bool enabled = false) :
        Module("Airjump", "Motion", "Jumps even if you don't touch the ground.", keybind, enabled)
    {

    }

    void onEvent(RenderContextEvent* event) override {
        if (!Global::getClientInstance()->getLocalPlayer())
            return;

        Global::getClientInstance()->getLocalPlayer()->setIsOnGround(true);
    }

    void onDisabled() override {
        if (!Global::getClientInstance()->getLocalPlayer())
            return;

        Global::getClientInstance()->getLocalPlayer()->setIsOnGround(false);
    }
};