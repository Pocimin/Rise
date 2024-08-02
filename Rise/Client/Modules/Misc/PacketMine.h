#pragma once

class PacketMine : public Module
{
public:
    PacketMine(int keybind = Keys::NONE, bool enabled = false) :
        Module("PacketMine", "Misc", "Mines the blocks nigger", keybind, enabled)
    {
    }

    Vector3<int> BlockPos = Vector3<int>(0, 0, 0);
    int BlockSide = 0;
    bool ShouldDestroy = false;

    void onEvent(PacketEvent* event) override {
        if (Global::getClientInstance()->getLocalPlayer() == nullptr)
            return;

        if (!ShouldDestroy) return;

        PlayerAuthInputPacket* packet = nullptr;

        Vector2<float> angle = Global::getClientInstance()->getLocalPlayer()->getStateVector()->Position.CalcAngle(Global::getClientInstance()->getLocalPlayer()->getStateVector()->Position, BlockPos.ToFloat());

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            packet = (PlayerAuthInputPacket*)event->Packet;
        }

        if (TimeUtils::hasTimeElapsed("PacketMine", 480, false)) {
            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (pkt) {
                    pkt->rotation.x = angle.x;
                    pkt->rotation.y = angle.y;
                    pkt->headYaw = angle.y;
                }
            }
        }
    }

    bool findBestTool(Block* block) {
        PlayerInventory* playerInventory = Global::getClientInstance()->getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        bool isAxe = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Wood;
        bool isDirt = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Dirt;
        bool isStone = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Stone;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (isAxe) {
                    if (stack->getItem()->isAxe()) {
                        if (previousSlot != i)
                            playerInventory->hotbarSlot = i;
                        return true;
                    }
                }
                if (isDirt) {
                    if (stack->getItem()->nameContains("shovel")) {
                        if (previousSlot != i)
                            playerInventory->hotbarSlot = i;
                        return true;
                    }
                }
                else {
                    if (stack->getItem()->isPickaxe()) {
                        if (previousSlot != i)
                            playerInventory->hotbarSlot = i;
                        return true;
                    }
                }
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) return;

        HitResult* HitResult = player->getLevel()->getHitResult();
        Block* block = Global::getClientInstance()->getBlockSource()->getBlock(HitResult->IBlockPos);

        bool isDestroyed = false;

        static bool startDestroying = false;

        if (Utils::leftClick) {
            if (block->GetBlockLegacy()->getBlockID() != 0) {
                //player->getGamemode()->startDestroyBlock(BlockPos, BlockSide, isDestroyed);
                BlockPos = HitResult->IBlockPos;
                BlockSide = HitResult->BlockFace;
                TimeUtils::resetTime("PacketMine");
                ShouldDestroy = true;
            }
        }

        if (startDestroying) {
            player->getGamemode()->startDestroyBlock(BlockPos, BlockSide, isDestroyed);
            startDestroying = false;
        }

        if (ShouldDestroy) {
            startDestroying = true;
            if (TimeUtils::hasTimeElapsed("PacketMine", 490, true)) {
                int Slot = player->getSupplies()->hotbarSlot;

                if (findBestTool(Global::getClientInstance()->getBlockSource()->getBlock(BlockPos))) {
                    player->getGamemode()->destroyBlock(BlockPos, BlockSide);
                }
                player->getSupplies()->hotbarSlot = Slot;

                ShouldDestroy = false;
            }
        }
    }
};