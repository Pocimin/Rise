#pragma once

class TestModule : public Module
{
public:
    TestModule(int keybind = 70, bool enabled = false) :
        Module("TestModule", "Motion", "a module for development purposes", keybind, enabled)
    {
        // a1 flight purposes
        // a2 soon for disabler shit
        // a3 soon for speed shit
        // a4 soon for funny packets :D
        addEnum("Interact", "(developer testing stuff)", { "a1" }, &mode);
        addSlider("Boost", "The boost duh", &speed, 0, 30);
        addSlider("Timer", "The timer duh", &timer, 1, 25);
        addSlider("Glider", "The glide nigga", &glide, -1, 1);
    }

private:
    int mode = 0;
    float speed = 0.7;
    float timer = 7.3f;
    float glide = -0.03f; // Make it glide a bit or u homeless and getting flagged
    

    bool ShouldFly = false; // We using bools cuz we are homeless
public:
    void onEnabled() override {
        if (!Global::getClientInstance()->getLocalPlayer()) return;

        Global::getClientInstance()->getTimerClass()->setMainTimerSpeed(20.f);
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getTimerClass())
            return;

        if (!Global::getClientInstance()->getLocalPlayer())
            return;

        Player* player = Global::getClientInstance()->getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();
    }

    void onEvent(PacketEvent* event) override {
        if (!Global::getClientInstance()->getLocalPlayer())
            return;

        //if (strcmp(event->Packet->getTypeName().getText(), "PlayerAuthInputPacket") != 0 && strcmp(event->Packet->getTypeName().getText(), "SubChunkRequestPacket") != 0) {
            //Global::getClientInstance()->getLocalPlayer()->displayClientMessage(Utils::combine(GRAY, "Packet -> ", RESET, event->Packet->getTypeName().getText()));
        //}
    }

    void onDisabled() override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getTimerClass())
            return;

        Global::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
    }
};