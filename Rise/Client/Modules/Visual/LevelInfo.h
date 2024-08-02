#pragma once

class LevelInfo : public Module
{
public:
    LevelInfo(int keybind = Keys::NONE, bool enabled = false) :
        Module("LevelInfo", "Visual", "Displays information about level", keybind, enabled)
    {
        addBool("Ping", "Displays your ping", &showPing);
        addBool("BPS", "Displays your bps", &showBPS);
        addBool("XYZ", "Displays your xyz pos", &showXYZ);
        addBool("SpeedSpells", "Displays your speed spells count", &showSpeedSpells);
        addBool("HealthSpells", "Displays your health spells count", &showHealthSpells);
        //addBool("ItemInfo", "Displays the item info for the item ur selecting", &showItemInfo);
        addBool("BlockInfo", "Displays the pointing block info", &showBlockInfo);
        addBool("Pearls", "Displays your pearl count", &showPearls);
        addBool("Arrows", "Displays your arrows count", &showArrows);
    }

    bool showPing = true;
    bool showBPS = true;
    bool showXYZ = false;
    bool showSpeedSpells = false;
    bool showHealthSpells = false;
    bool showItemInfo = false;
    bool showBlockInfo = false;
    bool showPearls = false;
    bool showArrows = false;

    int ping = 0;
    int BlockID = 0;
    int totalPearlCount = 0;
    int totalArrowCount = 0;

    std::vector<float> bpsQueue;
    std::string currBps = "";
    int tick = 0;

    void onEnabled() override {
        bpsQueue.clear();
        currBps = "0.0";
        tick = 0;
    }

    void onEvent(IntersectsTickEvent* event) override {
        auto* player = Global::getClientInstance()->getLocalPlayer();

        if (player == nullptr)
            return;

        PlayerInventory* supplies = player->getSupplies();
        auto n = supplies->hotbarSlot;
        Inventory* inv = supplies->inventory;
        ItemStack* stack = inv->getItem(n);

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inv->getItem(i);
            if (stack->item != nullptr) {
                if (stack->getItem()->nameContains("ender_pearl")) {
                    totalPearlCount = (int)(*(uint8_t*)(((uintptr_t)stack) + 0x22));
                }

                if (stack->getItem()->nameContains("arrow")) {
                    totalArrowCount = (int)(*(uint8_t*)(((uintptr_t)stack) + 0x22));
                }
            }
        }

        BlockID = Global::getClientInstance()->getBlockSource()->getBlock(player->getLevel()->getHitResult()->IBlockPos)->GetBlockLegacy()->getBlockID();

        if (player == nullptr)
        {
            currBps = "0.0";
            if (!bpsQueue.empty())
            {
                bpsQueue.clear();
            }
            tick = 0;
            return;
        }

        if (tick >= 20)
        {
            float total = 0.f;
            if (!bpsQueue.empty())
            {
                for (int i = 0; i < bpsQueue.size(); i++)
                {
                    if (i > 20)
                        break;
                    total += bpsQueue.at(i);
                    bpsQueue.erase(bpsQueue.begin());
                }
                total /= (bpsQueue.size() > 20 ? 20 : bpsQueue.size());
                total = truncf(total * 10) / 10;
            }
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << total;
            currBps = ss.str();
            //currBps = std::to_string(total);
            tick = 0;
            //bpsQueue.clear();

            bpsQueue.push_back(std::stof(
                std::to_string((int)player->getBlocksPerSecond()) + std::string(".") + std::to_string((int)(player->getBlocksPerSecond() * 10) - ((int)player->getBlocksPerSecond() * 10))
            ));
            return;
        }
        bpsQueue.push_back(std::stof(
            std::to_string((int)player->getBlocksPerSecond()) + std::string(".") + std::to_string((int)(player->getBlocksPerSecond() * 10) - ((int)player->getBlocksPerSecond() * 10))
        ));
        tick++;
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) return;

        //LevelInfo
        Vector3<float> currPos = player->getStateVector()->Position; // User's current position

        //std::string 
        std::string BlockIDLevelInfo = to_string(BlockID);
        std::string PingLevelInfo = "Ping: " + to_string(ping) + "ms";

        std::string BPSLevelInfo = "BPS: " + currBps;

        std::string XYZLevelInfo = "XYZ: " + std::to_string((int)floorf(currPos.x)) + " " + std::to_string((int)floorf(currPos.y)) + " " + std::to_string((int)floorf(currPos.z));

        int index = 0;

        if (showHealthSpells) {
            drawLevelInfo("Health spells: 0 (0 hearts)", index++);
        }
        
        if (showXYZ) {
            drawLevelInfo(XYZLevelInfo, index++);
        }
        
        if (showBPS) {
            drawLevelInfo(BPSLevelInfo, index++);
        }

        if (showSpeedSpells) {
            drawLevelInfo("Speed spells: 0", index++);
        }

        if (showPing) {
            drawLevelInfo(PingLevelInfo, index++);
        }
        
        /*if (showItemInfo) {
            drawLevelInfo(ItemIDLevelInfo, index++);
        }*/

        if (showBlockInfo) {
            drawLevelInfo(BlockIDLevelInfo, index++);
        }

        if (showPearls) {
            drawLevelInfo("Pearls: " + std::to_string(totalPearlCount), index++);
        }

        if (showArrows) {
            drawLevelInfo("Arrows: " + std::to_string(totalArrowCount), index++);
        }
        
        //drawLevelInfo(BlockIDLevelInfo, 5);
        //drawLevelInfo(ItemIDLevelInfo, 6);
    }

    void onEvent(PacketEvent* event) override {
        if (event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector != nullptr) {
            if (event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->JoinedIp == "") { // Checks if User is in a server
                ping = -1; // Sets the ping to 0 if user is in a world to avoid crashing
            }
            else {
                ping = event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->getPing();

            }
        }
        //ping = event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->getPing();
    }

    void drawLevelInfo(std::string tileDescr, int index)
    {
        Vector2<float> tilePos = Vector2<float>(5, (Global::getClientInstance()->getGuiData()->mcResolution.y - 25) - (index * 17.f));

        ImRenderUtil::drawText(tilePos, &tileDescr, UIColor(255, 255, 255), 1.f, 1, true);

        float width = ImRenderUtil::getTextWidth(&tileDescr, 1);

        if (size.x < width)
            size.x = width;

        location = tilePos;
        size.y = (index + 1) * 10;
    }
};