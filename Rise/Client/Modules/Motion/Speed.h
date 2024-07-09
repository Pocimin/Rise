#pragma once

class Speed : public Module
{
public:
    Speed(int keybind = Keys::X, bool enabled = false) :
        Module("Speed", "Motion", "Bhop like the flash.", keybind, enabled)
    {
        addEnum("Mode", "The mode of the speed", { "Bunnyhop", "Friction", "Flareon", "Static" }, &mode);
        addSlider("Speed", "How fast you will go", &speed, 1, 10);
        addSlider("SwiftSpeed", "How fast you will go when boosting", &swiftspeed, 1, 10);
        addSlider("Height", "(BunnyHop only)", &height, 0, 7);
        addSlider("Friction", "How fast you will slowdown off ground", &friction, 0, 10);
        addBool("AutoSwiftness", "Automaticly uses swiftness spell books", &autoswiftness);
        addBool("Timer boost", "Increase the game tick rate", &timerBoost);
        addSlider("Timer", "Increase the game timer", &timerSpeed, 0, 60);
        addBool("Fast Fall", "Fast falls into the ground", &fastfall);
        addSlider("Fall Speed", "The amount speed to fall (only when fast fall enabled)", &fastfallspeed, 2, 50);
        addBool("DamageBoost", "Boost speed when you knockback", &damageBoost);
        addSlider("DamageBoostSpeed", "How fast you will go when you get damage", &damageBoostSpeed, 0, 5);
    }

    float speed = 3.5f;
    float swiftspeed = 7.5f;
    float height = 4.2f;
    float friction = 1.f;

    bool autoswiftness = false;

    bool timerBoost = false;
    float timerSpeed = 20;

    bool fastfall = false;
    float fastfallspeed = 20;

    //HiveLow
    bool fallen = false;

    // DamageBoost
    bool damageBoost = false;
    float damageBoostSpeed = 1.5f;

    int mode = 0;

    void onEnabled() override {
        Global::lastLerpVelocity = NULL;
    }

    void doSwiftness(Player* player) {
        PlayerInventory* playerInventory = player->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        for (int n = 0; n < 36; n++) {
            ItemStack* stack = inventory->getItem(n);
            if (stack->item != nullptr) {
                std::string ItemName = stack->getItem()->name;
                if (stack->customNameContains("Spell of Swiftness")) {
                    if (previousSlot != n) {
                        playerInventory->hotbarSlot = n;
                        player->getGamemode()->baseUseItem(*stack);
                        playerInventory->hotbarSlot = previousSlot;
                    }
                }
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Global::getClientInstance() || !Global::getClientInstance()->getLocalPlayer() || !Global::getClientInstance()->getLocalPlayer()->getStateVector() || !Global::getClientInstance()->getTimerClass() || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
            return;

        Player* player = Global::getClientInstance()->getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();


        auto onGround = player->isOnGround();

        bool keyPressed = player->getMoveInputHandler()->isPressed();
        if (keyPressed) player->setSprinting(true);

        static float currentSpeed;
        static float lastSpeed;

        float yaw = (float)player->getMovementInterpolator()->Rotations.y;

        switch (mode) {
        case 0: //BunnyHop
            // Are we holding movement keys?
            if (keyPressed)
            {
                // Should we jump?
                if ((height / 10) > 0 && onGround)
                {
                    state->Velocity.y += 1;
                    state->Velocity.y = height / 7;
                    player->jumpFromGround();
                    PlayerActionPacket
                }
                MovementUtils::setSpeed(speed / 10);
            }
            break;
        case 1: // Friction
            // Damage Boost
            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                lastSpeed = boostedSpeed;
                Global::lastLerpVelocity = NULL;
            }
            if (!lastSpeed) lastSpeed = speed / 10;
            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 0, true)) {
                        currentSpeed = speed / 10;
                        lastSpeed = speed / 10;
                        //if ((height / 10) <= 3.9 && (height / 10) >= 4.1) {
                        player->jumpFromGround();
                        //}
                        //else {
                            //state->Velocity.y += height / 10;
                        //}
                    }
                }
                else
                {
                    Vector3<float> velocity = state->Velocity;
                    if (abs(velocity.magnitudexz()) > lastSpeed) {
                        currentSpeed = velocity.magnitudexz();
                    }
                    lastSpeed = velocity.magnitudexz();
                    currentSpeed = currentSpeed * (friction / 10);
                    MovementUtils::setSpeed(currentSpeed);
                }
            }
            break;
        case 2: // Flareon
            // Damage Boost
            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                MovementUtils::setSpeed(currentSpeed);
                Global::lastLerpVelocity = NULL;
            }
            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 0, true)) {
                        Vector3<float> velocity = state->Velocity;
                        currentSpeed = velocity.magnitudexz();
                        player->jumpFromGround();
                    }
                }
                else
                {
                    Vector3<float> velocity = state->Velocity;
                    currentSpeed = velocity.magnitudexz();
                    MovementUtils::setSpeed(currentSpeed);
                }
            }
            //FastFall
            if (state->Velocity.y >= -0.2) {
                fallen = false;
            }
            else if (!fallen) {
                state->Velocity.y *= 1.75f;
                fallen = true;
            }
            break;
        case 3: // SprintHop
            // Damage Boost
            player->setSprinting(true);

            static bool gotBoost = false;

            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                lastSpeed = boostedSpeed;
                gotBoost = true;
                Global::lastLerpVelocity = NULL;
            }

            if (Global::effectSwiftness) {
                if (!TimeUtils::hasTimeElapsed("SwiftnessBoost", 5000, false)) {
                    currentSpeed = swiftspeed / 10;
                    lastSpeed = swiftspeed / 10;
                }

                if (TimeUtils::hasTimeElapsed("SwiftnessBoost", 5000, false)) {
                    Global::effectSwiftness = false;
                }
            }

            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 0, true)) {
                        gotBoost = false;
                        player->jumpFromGround();
                    }
                }
                else
                {
                    if (autoswiftness) {
                        doSwiftness(player);
                    }

                    Vector3<float> velocity = state->Velocity;
                    if (abs(velocity.magnitudexz()) > lastSpeed) {
                        currentSpeed = velocity.magnitudexz();
                    }
                    lastSpeed = velocity.magnitudexz();
                    currentSpeed = currentSpeed * (friction / 10);
                    if (gotBoost || Global::effectSwiftness) {
                        MovementUtils::setSpeed(currentSpeed);
                    }
                }
            }
            break;
        }
    }

    void onDisabled() override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getTimerClass())
            return;
    }
};