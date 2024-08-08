#pragma once
// why is velocity empty -Jordan 8/7/2024
// created by Jordan on 8//7/2024
class Velocity : public Module
{
public:
    Velocity(int keybind = 7, bool enabled = true) :
        Module("Velocity", "Motion", "Modify knockback received by the player.", keybind, enabled)
    {
        addSlider("Horizontal", "Modify horizontal knockback", &horizontal, 0.0f, 1.0f);
        addSlider("Vertical", "Modify vertical knockback", &vertical, 0.0f, 1.0f);
        addBool("NoFall", "Disable fall damage knockback", &noFall);
    }

    float horizontal = 1.0f;  // Horizontal velocity multiplier
    float vertical = 1.0f;    // Vertical velocity multiplier
    bool noFall = false;      // Disable fall damage knockback

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Global::getClientInstance() || !Global::getClientInstance()->getLocalPlayer() || !Global::getClientInstance()->getLocalPlayer()->getStateVector() || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
            return;

        Player* player = Global::getClientInstance()->getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        if (this->isEnabled()) {
            if (player->isOnGround()) {
                // Modify horizontal and vertical velocity
                state->Velocity.x *= horizontal;
                state->Velocity.z *= horizontal;
                state->Velocity.y *= vertical;

                if (noFall && state->Velocity.y < 0) {
                    // Disable fall knockback if enabled
                    state->Velocity.y = 0.0f;
                }
            }
        }
    }

    void onDisabled() override {
        // Reset to default values when disabled
        horizontal = 1.0f;
        vertical = 1.0f;
        noFall = false;
    }
};
