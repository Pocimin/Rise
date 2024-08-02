#pragma once

class Phase : public Module
{
public:
    Phase(int keybind = 7, bool enabled = false) :
        Module("Phase", "Player", "Allow phasing through blocks", keybind, enabled)
    {
        addEnum("Mode", "The mode for the delay", { "Horizontal", "Vertical" }, &phasemode);
        addSlider("Speed", "The speed of vertical movement", &speed, 0.2, 1);
    }

private:
    int phasemode = 0;

    float speed = 0.4;

    float oldPosUpperY;
public:
    void onEnabled() override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;
        oldPosUpperY = player->getAABBShape()->PosUpper.y;
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getLocalPlayer() || !Global::getClientInstance()->getLocalPlayer()->getStateVector())
            return;

        Player* player = Global::getClientInstance()->getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();
        BlockSource* source = Global::getClientInstance()->getBlockSource();
        Vector3<int> playerPos = state->Position.ToInt();
        playerPos.y = state->Position.floor().y - 1.5f;

        if (phasemode == 0) {
            player->getAABBShape()->PosUpper.y = player->getAABBShape()->PosLower.y;
        }

        if (phasemode == 1) {
            if (0 < player->getStateVector()->Velocity.y) 
                player->getStateVector()->Velocity.y = 0.f;

            if (source->getBlock(playerPos)->GetMaterialType() == MaterialType::Air && source->getBlock(playerPos.add(Vector3<int>(0, -1, 0)))->GetMaterialType() == MaterialType::Air) 
                return;

            player->getStateVector()->Velocity = Vector3<float>(0.f, 0.f, 0.f);

            //if (!player->isOnGround()) {
                //state->Velocity.y -= 0.4;
            //}

            player->getAABBShape()->PosUpper.y = 0.f;
            if (Global::Keymap[VK_SPACE]) {
                player->getStateVector()->Velocity.y += speed;
            }
            else if (Global::Keymap[VK_SHIFT]) {
                player->getStateVector()->Velocity.y -= speed;
            }
        }
    }

    void onEvent(RenderContextEvent* event) override
    {
        Player* player = Global::getClientInstance()->getLocalPlayer();
        AABBShapeComponent* aabb = player->getAABBShape();

        if (!player) { return; }

        if (phasemode == 0) {
            aabb->PosUpper.y = aabb->PosLower.y;
        }

        if (phasemode == 1) {
            aabb->PosUpper.y = aabb->PosLower.y - 3;
        }
       
    }

    void onDisabled() override
    {
        Player* player = Global::getClientInstance()->getLocalPlayer();
        AABBShapeComponent* aabb = player->getAABBShape();

        if (!player) { return; }

        aabb->PosUpper.y = aabb->PosLower.y + 1.8f;

        //player->SetPos(aabb->r_Pos_Lower);
    }
};