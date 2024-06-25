#pragma once

class Disabler : public Module
{
public:
    Disabler(int keybind = 7, bool enabled = true) :
        Module("Disabler", "Misc", "Disable certain anticheats.", keybind, enabled)
    {
        addEnum("Mode", "The mode of which the disabler will work", { "Flareon", "Sentinel", "AAC" }, &mode);
    }

    int mode = 0;
    bool displayFlags = true;

    void onEnabled() override {

    }

    void onEvent(PacketEvent* event) override {
        if (mode == 1) {
            if (strcmp(event->Packet->getTypeName().getText(), "NetworkStackLatencyPacket") == 0) {
                *event->cancelled = true;
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        ClientInstance* instance = Global::getClientInstance();
        LoopbackPacketSender* sender = instance->getLoopbackPacketSender();

        if (!sender || !instance || !instance->getLocalPlayer())
        {
            return;
        }
    }

    void onEvent(RenderContextEvent* event) override {
        if (Global::getClientInstance()->getLocalPlayer() == nullptr)
            return;

        Player* ent = Global::getClientInstance()->getLocalPlayer();

        //ChatUtils::sendMessage(Utils::combine("Layer: ", Global::LayerName));
    }
};