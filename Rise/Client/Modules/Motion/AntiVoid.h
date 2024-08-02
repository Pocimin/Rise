#pragma once

class AntiVoid : public Module
{
public:
    AntiVoid(int keybind = Keys::NONE, bool enabled = false) :
        Module("AntiVoid", "Motion", "Automatically avoids falling into the void or off cliffs", keybind, enabled)
    {
        addSlider("Distance", "Distance to fall before teleporting", &distance, 0, 1);
    }

    float distance = 1;

    Block* getStandingBlock(Vector3<float> position)
    {
        BlockSource* src = Global::getClientInstance()->getBlockSource();

        if (!src)
            return nullptr;

        return src->getBlock(position.ToInt());
    }

    void onEvent(RenderContextEvent* event) override
    {
        if (!Global::getClientInstance()->getLocalPlayer() || !Global::getClientInstance()->getBlockSource())
            return;

        Player* player = Global::getClientInstance()->getLocalPlayer();
        BlockSource* src = Global::getClientInstance()->getBlockSource();

        static Vector3<float> savePos = player->getStateVector()->Position;
        Vector3<float> blockBelow = player->getStateVector()->Position;

        // Save the last onGround pos
        if (player->isOnGround() && getStandingBlock(player->getStateVector()->Position)->GetMaterialType() != MaterialType::Air)
        {
            savePos = blockBelow;
        }

        bool blockBelowAir = false;
        for (int y = blockBelow.y; y >= -65; --y)
        {
            if (src->getBlock(Vector3<int>(blockBelow.x, y, blockBelow.z))->GetMaterialType() == MaterialType::Air)
            {
                blockBelowAir = true;
            }
            else
            {
                blockBelowAir = false;
                break;
            }
        }

        if (blockBelowAir)
        {
            player->getStateVector()->Position = (savePos);
        }
    }
};