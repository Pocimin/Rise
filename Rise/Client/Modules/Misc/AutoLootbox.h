#pragma once

class AutoLootbox : public Module
{
public:
    AutoLootbox(int keybind = Keys::NONE, bool enabled = false) :
        Module("AutoLootbox", "Misc", "Hits every entity around you.", keybind, enabled)
    {
        addSlider("Distance", "The distance of attacking", &range, 3, 10);
    }

private:
    float range = 6;
public:
    std::vector<Actor*> targetList;
    void updateTargetList() {
        auto instance = Global::getClientInstance();
        auto localPlayer = instance->getLocalPlayer();
        auto list = localPlayer->getLevel()->getRuntimeActorList();
        auto lpPos = localPlayer->getStateVector()->Position;
        for (Actor* actor : list) {
            if (actor != localPlayer && actor->getEntityTypeId() == 256) {
                float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
                if (dist <= range) {
                    targetList.push_back(actor);
                }
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        targetList.clear();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
        {
            return;
        }

        GameMode* gm = player->getGamemode();

        if (!gm)
            return;

        updateTargetList();

        if (!targetList.empty()) {
            player->getLevel()->HitType = 1;

            for (auto* actor : targetList) {
                gm->attack(*actor);
            }
        }
    }
};