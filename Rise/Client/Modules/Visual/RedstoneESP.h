#pragma once

class RedstoneESP : public Module
{
public:
    RedstoneESP(int keybind = Keys::NONE, bool enabled = false) :
        Module("RedstoneESP", "Visual", "Esp for redstone ores", keybind, enabled)
    {
        addBool("Lit Redstone", "include lit redstone", &litredstone);
        addSlider("Range", "How far around you is esp rendered", &range, 1, 100);
    }

    bool litredstone = true;
    float range = 20;

    bool Render = false;

    vector<Vector3<int>> blocks;
    vector<Vector3<int>> blocksFuni;

    Vector3<float> blockPos;

    Vector3<int> getBlockPosition()
    {
        auto localPlayer = Global::getClientInstance()->getLocalPlayer();
        //if (localPlayer == nullptr) return;

        Vector3<float> pos = localPlayer->getAABBShape()->PosLower;

        return { (int)floor(pos.x), (int)floor(pos.y), (int)floor(pos.z) };
    }

    void onEvent(RenderContextEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) {
            return;
        }

        Vector3<float> pos = player->getStateVector()->Position;

        for (int x = -range; x <= range; x++) {
            for (int z = -range; z <= range; z++) {
                for (int y = -range; y <= range; y++) {
                    //blocks.push_back(Vector3<int>(x, y, z));

                    int id = (int)Global::getClientInstance()->getBlockSource()->getBlock(player->getStateVector()->Position.add(Vector3<int>(x, y, z).ToFloat()).ToInt())->GetBlockLegacy()->getBlockID();
                    // bool Render = false;

                    if (id == 73 || id == 74) {
                        if (blocks.empty())
                        blocks.push_back(Vector3<int>(x, y, z));
                    }
                    else {
                        continue;
                    }
                }
            }
        }

        /*for (const Vector3<int>& offset : blocks) {
            blockPos = player->getStateVector()->Position.add(Vector3<int>(offset).ToFloat());

            Block* block = Global::getClientInstance()->getBlockSource()->getBlock(blockPos.ToInt());

            if (block->GetBlockLegacy()->getBlockID() != 73 && block->GetBlockLegacy()->getBlockID() != 74) continue;

            Vector3<float> angle = blockPos;
            angle = angle.floor();
            angle.x += 1;
            angle.y += 1;
            angle.z += 1;

            //ImRenderUtil::drawBox(Vector3<float>(0, 0, 0), angle, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        }*/
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) {
            return;
        }

        for (const Vector3<int>& offset : blocks) {
            blockPos = player->getStateVector()->Position.add(Vector3<int>(offset).ToFloat());

            Block* block = Global::getClientInstance()->getBlockSource()->getBlock(blockPos.ToInt());

            if (block->GetBlockLegacy()->getBlockID() != 73 && block->GetBlockLegacy()->getBlockID() != 74) continue;

            Vector3<float> angle = blockPos;
            angle = angle.floor();
            angle.x += 1;
            angle.y += 1;
            angle.z += 1;

            ImRenderUtil::drawBox(blockPos, angle, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        }
    }
};