#pragma once

class AutoQueue : public Module
{
public:
    AutoQueue(int keybind = Keys::NONE, bool enabled = false) :
        Module("AutoQueue", "Misc", "Hits every entity around you.", keybind, enabled)
    {
        addEnum("Mode", "When to reque into a game", { "Death", "Game OVER" }, &Mode);
    }

private:
    int Mode = 0;

    std::string gamemode = "";
    int ticks = 0;
public:
    void onEvent(ActorBaseTickEvent* event) override {
        // when low on health we don't have this yet
        auto player = Global::getClientInstance()->getLocalPlayer();

        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) return;

        PlayerInventory* playerInventory = player->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        static bool foundItem = false;

        for (int n = 0; n < 36; n++) {
            ItemStack* stack = inventory->getItem(n);
            if (stack->item != nullptr) {
                std::string ItemName = stack->getItem()->name;
                if (stack->getItem()->nameContains("heart_of_the_sea") && Mode == 0) {
                    foundItem = true;
                    ticks++;
                }
                else {
                    ticks = 0;
                }
            }
        }

        if (foundItem && ticks == 3) {
            std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(77);
            auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
            command_packet->Command = "/q sky";

            command_packet->Origin.mType = CommandOriginType::Player;

            command_packet->InternalSource = true;
            Global::getClientInstance()->getLoopbackPacketSender()->sendToServer(command_packet);

            ChatUtils::sendMessage("Queueing you into a game of \"SKY\"...");
            foundItem = false;
        }
    }

    void onEvent(PacketEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;

        static bool triggered = false;

        if (event->Packet->getId() == PacketID::PlaySoundPacket) {
            auto* pkt = reinterpret_cast<PlaySoundPacket*>(event->Packet);

            if (pkt->mName == "raid.horn" || pkt->mName == "ui.toast.challenge_complete_java") {
                std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                command_packet->Command = "/q sky";

                command_packet->Origin.mType = CommandOriginType::Player;

                command_packet->InternalSource = true;
                Global::getClientInstance()->getLoopbackPacketSender()->sendToServer(command_packet);
            }
        }
    }
};