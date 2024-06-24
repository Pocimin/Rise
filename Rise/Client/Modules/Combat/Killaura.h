#pragma once

class Killaura : public Module
{
public:
    Killaura(int keybind = 80, bool enabled = false) :
        Module("Killaura", "Combat", "Hits every entity around you.", keybind, enabled)
    {
        addEnum("Mode", "How many entities should be attacked", { "Single", "Multi", "Switch" }, &switchMode);
        addEnum("Rotations", "The style of the rotations", { "Simple" }, &rotations);
        addEnum("Side", "The side for the rotations", { "Client", "Server" }, &Side);
        addEnum("Swing", "The way the players arm swing", { "Normal", "Disabled" }, &Swing);
        addEnum("AutoSlot", "The way the players arm swing", { "Disabled", "Switch", "Spoof" }, &AutoSlot);
        addSlider("Distance", "The distance of attacking", &range, 3, 10);
        addSlider("APS", "attack per second", &APS, 0, 20);
        addBool("Strafe", "Strafe the player towards the entity", &Strafe);
        addBool("Timed swing", "Times the swing with the attack", &Timedswing);
        addBool("Ember Sword", "Automaticly uses the ember sword (golden_swords)", &emberSword);
        addBool("Nemo", "Automaticly uses nemo fishes (tropical_fish)", &Nemo);
        addBool("Silent", "Make the switch for nemo and ember sword silent", &silentSwitch);
        addBool("Projectiles", "Throws snowballs at the target", &projectiles);
        addBool("Hive", "Prevents you from being kicked by the anticheat for aura", &Hive);
        addSlider("Speed", "The speed of the animations colour", &AnimationSpeed, 0, 20);
    }

private:
    int switchMode = 0;
    int rotations = 0;
    int Side = 0;
    int Swing = 0; // enum
    int AutoSlot = 0; // enum
    float range = 4;
    float APS = 10;
    int countAPS = 0;
    bool Strafe = false;
    bool Timedswing = true;
    bool emberSword = false;
    bool Nemo = false;
    bool silentSwitch = false;
    bool projectiles = false;
    bool Hive = false;

    int betterSwordSlot = -1;

    int beforeSlot = 0;

    int targetIndex = 0;
    float AnimationSpeed = 10;

    static inline Vector3<float> targetRenderPos;
    static inline float radius = 0;
    static inline int segments = 300;
    static inline float anim = 0;
    const float coolAnim = (0.9f + 0.9f * sin((anim / 60) * PI * 1)) * 2;
    static inline float yPos = targetRenderPos.y; // Create a Y pos for the head pos
public:

    const static Vector2<float> CalcAngle(Vector3<float> ths, Vector3<float> dst)
    {
        float deltaX = dst.x - ths.x;
        float deltaZ = dst.z - ths.z;
        float deltaY = dst.y - ths.y;
        float deltaXZ = hypot(deltaX, deltaZ);

        float yaw = atan2(-deltaX, deltaZ);

        float yawDegrees = yaw * (180 / PI);
        float pitch = atan2(deltaY, deltaXZ) * (180 / PI);

        return Vector2<float>(-pitch, yawDegrees);
    }

    void onEnabled() override {
        Global::isAttacking = false;

        if (Global::getClientInstance()->getLocalPlayer() != nullptr) {
            beforeSlot = Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot;
            targetList.clear();
            botList.clear();
        }
        countAPS = 0;
    }

    void onDisabled() override {
        Global::isAttacking = false;
        Global::ShouldBlock = false;
    }

    std::vector<Actor*> targetList;
    std::vector<Actor*> botList;
    void updateTargetList() {
        auto instance = Global::getClientInstance();
        auto localPlayer = instance->getLocalPlayer();
        auto list = localPlayer->getLevel()->getRuntimeActorList();
        auto lpPos = localPlayer->getStateVector()->Position;
        for (Actor* actor : list) {
            if (actor != localPlayer && actor->isAlive() && !actor->isBot()) { //  && !actor->isBot()
                float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
                if (dist <= range) {
                    if (getModuleByName("teams")->isEnabled()) {
                        if (!actor->isTeammate(localPlayer)) {
                            targetList.push_back(actor);
                        }
                    }
                    else {
                        targetList.push_back(actor);
                    }
                }
            }
        }
    }

    void updateBotList(Actor* target) {
        auto instance = Global::getClientInstance();
        auto localPlayer = instance->getLocalPlayer();
        auto list = localPlayer->getLevel()->getRuntimeActorList();
        auto targetPos = target->getAABBShape()->PosLower;
        for (Actor* actor : list) {
            Vector2<float> hitbox = actor->getAABBShape()->Hitbox;
            if (hitbox.x >= 0.70f && hitbox.x <= 0.80f && hitbox.y >= 2.0f && hitbox.y <= 2.2f) { //  && !actor->isBot()
                float dist = targetPos.distance(actor->getStateVector()->Position);
                if (2 <= range) {
                    botList.push_back(actor);
                }
            }
        }
    }

    bool findSword() {
        PlayerInventory* playerInventory = Global::getClientInstance()->getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;
        int slot = previousSlot;

        float currentSwordValue = 0;
        bool isHotbar = false;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                currentSwordValue = 0;
                if (stack->getItem()->isSword()) {
                    if (stack->getItem()->getSwordValueFromName() + stack->getEnchantValue(9) > currentSwordValue) {
                        currentSwordValue = stack->getItem()->getSwordValueFromName() + stack->getEnchantValue(9);
                        betterSwordSlot = i;

                        return true;
                    }
                }
            }
        }

        return false;
    }

    void AutoSlotManagement() {
        bool findsword = findSword();
        if (AutoSlot == 1 && findsword) {
            Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot = betterSwordSlot;
        }

        if (AutoSlot == 2 && findsword) {
            Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot = betterSwordSlot;
        }
    }

    void SilentAutoSlot() {
        bool findsword = findSword();

        if (AutoSlot == 2 && findsword) {
            Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot = beforeSlot;
        }
    }

    void Projectiles(Player* player) {
        PlayerInventory* playerInventory = Global::getClientInstance()->getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        int slot = previousSlot;

        for (int n = 0; n < 36; n++) {
            ItemStack* stack = inventory->getItem(n);
            if (stack->item != nullptr) {
                std::string ItemName = stack->getItem()->name;
                if (stack->getItem()->nameContains("snowball")) {
                    if (previousSlot != n) {
                        if (TimeUtils::hasTimeElapsed("Projectiles", 170, true)) {
                            playerInventory->hotbarSlot = n;
                            player->getGamemode()->baseUseItem(*stack);
                            playerInventory->hotbarSlot = previousSlot;
                        }
                    }
                }
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        targetList.clear();
        botList.clear();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
        {
            return;
        }

        if (getModuleByName("regen")->isEnabled() && !Global::shouldAttack) {
            return;
        }

        GameMode* gm = player->getGamemode();

        if (!gm)
            return;

        findSword();

        updateTargetList();

        if (!targetList.empty()) {
            countAPS++;
            Vector3<float> localPos = player->getStateVector()->Position;
            Vector3<float> playerPos = targetList[0]->getStateVector()->Position;
            Vector2<float> angle = CalcAngle(localPos, playerPos);
            if (1000 / APS <= countAPS * 50) {
                if (Swing == 0) { player->swing(); }

                Global::ShouldBlock = true;
                player->getLevel()->HitType = 1;

                AutoSlotManagement();

                if (projectiles) {
                    Projectiles(player);
                }

                updateBotList(targetList[0]);

                if (Hive && !botList.empty()) {
                    Global::isAttacking = true;
                    gm->attack(*botList[0]);
                }

                if (switchMode == 0) { //Single
                    Global::isAttacking = true;
                    gm->attack(*targetList[0]);
                }
                else if (switchMode == 1) { //Multi
                    for (auto* actor : targetList) {
                        Global::isAttacking = true;
                        gm->attack(*actor);
                    }
                }
                else if (switchMode == 2) { // Switch
                    Global::isAttacking = true;
                    targetIndex++;
                    if (targetIndex >= targetList.size())
                        targetIndex = 0;

                    gm->attack(*targetList[targetIndex]);
                }
                SilentAutoSlot();
                countAPS = 0;
            }
            Global::ShouldBlock = true;
            if (Strafe) {
                player->getMovementInterpolator()->Rotations = angle;
            }
        }
        else {
            Global::ShouldBlock = false;
            Global::isAttacking = false;
            countAPS = 0;
        }
    }

    void onEvent(IntersectsTickEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys() || Side != 0)
        {
            return;
        }

        if (getModuleByName("regen")->isEnabled() && !Global::shouldAttack) {
            return;
        }

        if (!targetList.empty()) {
            if (radius < 0.8)
                radius += 0.05;

            if (radius >= 0.8)
                radius -= 0.05;

            static float anim = 0;
            anim++;
            const float coolAnim = (0.9f + 0.9f * sin((anim / 60) * PI * 1)) * 2;

            auto yPos = targetRenderPos.y; // Create a Y pos for the head pos

            yPos -= 1.6f; // Decrease it by the height
            yPos += coolAnim / 2;

            targetRenderPos.y = yPos;

            if (switchMode == 0) {
                Vector3<float> localPos = player->getStateVector()->Position;
                Vector3<float> playerPos = targetList[0]->getStateVector()->Position;
                Vector2<float> angle = CalcAngle(localPos, playerPos);

                targetRenderPos = targetList[0]->getRenderPosition();

                if (rotations == 0) { // Simple
                    player->setPitch(angle.x);
                    player->setXHeadRotation(angle.y);
                    player->setBodyRotation(angle.y);
                }
            }
            else if (switchMode == 1) {
                for (auto* actor : targetList) {
                    Vector3<float> localPos = player->getStateVector()->Position;
                    Vector3<float> playerPos = actor->getStateVector()->Position;
                    Vector2<float> angle = CalcAngle(localPos, playerPos);

                    targetRenderPos = actor->getRenderPosition();

                    if (rotations == 0) { // Simple
                        player->setPitch(angle.x);
                        player->setXHeadRotation(angle.y);
                        player->setBodyRotation(angle.y);
                    }
                }
            }
            else if (switchMode == 2) { // Switch
                Vector3<float> localPos = player->getStateVector()->Position;
                Vector3<float> playerPos = targetList[targetIndex]->getStateVector()->Position;
                Vector2<float> angle = CalcAngle(localPos, playerPos);

                targetRenderPos = targetList[targetIndex]->getRenderPosition();

                if (rotations == 0) { // Simple
                    player->setPitch(angle.x);
                    player->setXHeadRotation(angle.y);
                    player->setBodyRotation(angle.y);
                }
            }
        }
        else {
            if (radius > 0) {
                radius -= 0.05;
            }
        }
    }

    void onEvent(PacketEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
        {
            return;
        }

        if (getModuleByName("regen")->isEnabled() && !Global::shouldAttack) {
            return;
        }

        PlayerAuthInputPacket* packet = nullptr;

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            packet = (PlayerAuthInputPacket*)event->Packet;
        }
        if (!targetList.empty()) {
            if (switchMode == 0) {
                Vector3<float> localPos = player->getStateVector()->Position;
                Vector3<float> playerPos = targetList[0]->getStateVector()->Position;
                Vector2<float> angle = CalcAngle(localPos, playerPos);

                if (rotations == 0) { // Simple
                    if (packet != nullptr) {
                        packet->rotation.x = angle.x;
                        packet->rotation.y = angle.y;
                        packet->headYaw = angle.y;
                    }
                }
            }
            else if (switchMode == 1) {
                for (auto* actor : targetList) {
                    Vector3<float> localPos = player->getStateVector()->Position;
                    Vector3<float> playerPos = actor->getStateVector()->Position;
                    Vector2<float> angle = CalcAngle(localPos, playerPos);

                    if (rotations == 0) { // Simple
                        if (packet != nullptr) {
                            packet->rotation.x = angle.x;
                            packet->rotation.y = angle.y;
                            packet->headYaw = angle.y;
                        }
                    }
                }
            }
            else if (switchMode == 2) {
                for (auto* actor : targetList) {
                    Vector3<float> localPos = player->getStateVector()->Position;
                    Vector3<float> playerPos = targetList[targetIndex]->getStateVector()->Position;
                    Vector2<float> angle = CalcAngle(localPos, playerPos);

                    if (rotations == 0) { // Simple
                        if (packet != nullptr) {
                            packet->rotation.x = angle.x;
                            packet->rotation.y = angle.y;
                            packet->headYaw = angle.y;
                        }
                    }
                }
            }
        }
    }

    void onEvent(ImGuiRenderEvent* event) override {
        if (getModuleByName("clickgui")->isEnabled()) return;

        auto player = Global::getClientInstance()->getLocalPlayer();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
        {
            return;
        }

        GameMode* gm = player->getGamemode();

        if (!gm)
            return;

        static float ZoomAnimations = radius;
        ZoomAnimations = Math::animate(radius, ZoomAnimations, ImRenderUtil::getDeltaTime() * 15.f / 10);

        if (radius > 0.01) {
            ImRenderUtil::drawRing3D(targetRenderPos, ZoomAnimations, segments, AnimationSpeed / 10);
        }
    }
};