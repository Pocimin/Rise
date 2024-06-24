#pragma once

class Step : public Module
{
public:
    Step(int keybind = Keys::NONE, bool enabled = false) :
        Module("Step", "Motion", "Stepping on blocks lmao", keybind, enabled)
    {
        addSlider("Height", "The maximum height which you will automatically step.", &height, 1, 5);
    }

    float height = 1.3;

    void onEvent(RenderContextEvent* event) override
    {
        if (Global::getClientInstance()->getLocalPlayer() == nullptr)
            return;

       Global::getClientInstance()->getLocalPlayer()->getComponent<MaxAutoStepComponent>()->maxAutoStep = height;
    }

    void onDisabled() override {
        if (Global::getClientInstance()->getLocalPlayer() != nullptr)
            Global::getClientInstance()->getLocalPlayer()->getComponent<MaxAutoStepComponent>()->maxAutoStep = 0.5625f;
    }
};