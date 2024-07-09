#pragma once

class Regen : public Module
{
public:
    Regen(int keybind = Keys::NUM_0, bool enabled = false) :
        Module("Regen", "Misc", "Mines redstones in the hive for you to regenerate.", keybind, enabled)
    {
        addEnum("Mode", "The mode for the delay", { "Milliseconds" }, &DelayMode);
        addEnum("Side", "The side for the rotations", { "Client", "Server" }, &Side);
        addSlider("Range", "How far around you find redstones", &range, 1, 10);
        addSlider("Time MS", "The delay for breaking in millisecounds", &destroyMs, 1, 1000);
        addSlider("UnExposed MS", "The delay for breaking blocks that sorround the redstone in millisecounds", &blockDestroyMs, 1, 1000);
        addBool("Exposed Only", "Only mine redstone ores that aren't covered.", &exposedOnly);
        //addBool("Queued", "Queues redstone for you to mine later.", &Queued);
        //addBool("Ignore Covered", "Ignore redstone if it's covered.", &IgnoreCovered);
        //addBool("Legit Mine", "Sets your mining style to legit", &LegitMine);
        addBool("Render", "Render the redstone counter", &render);
    }

private:
    int DelayMode = 0;
    int Side = 0;
    float range = 5;
    bool render = true;
    bool Queued = false;
    bool IgnoreCovered = false;
    bool exposedOnly = true;
    bool LegitMine = true;

    // Uneditable stuff
    Vector3<int> miningBlockPos;
    bool isRedstoneGettingDestroyed = false; // Is The RedstoneOre destroying?
    bool isExposed = false; // Is The RedstoneOre exposed?
    int RedstoneSide = 0; // The redstone ore's side.
    int previousSlot = 0; // The slot before destroying.
    int animationsTime = 0;
    float absorption = 0;

    float destroyMs = 800; // you can use getDestroySpeed function instead
    float blockDestroyMs = 800; // you can use getDestroySpeed function instead
    float currentDestroyMs = 0;

    bool Covered = false;

    bool hasPacketSent = false;
    bool shouldSetBackSlot = false;
    bool hasSelectedSlot = false;
    int currentPacketSlot = 0;
public:
    //Functions
    const static Vector2<float> CalcAngleRegen(Vector3<float> ths, Vector3<float> dst)
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

    static void ImScaleStart()
    {
        scale_start_index = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
    }

    static inline int scale_start_index;

    static ImVec2 ImScaleCenter()
    {
        ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

        const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
        for (int i = scale_start_index; i < buf.Size; i++)
            l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

        return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
    }

    static void ImScaleEnd(float scaleX, float scaleY, ImVec2 center = ImScaleCenter())
    {
        auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;

        for (int i = scale_start_index; i < buf.Size; i++)
        {
            ImVec2 pos = buf[i].pos - center;
            pos.x *= scaleX;
            pos.y *= scaleY;
            buf[i].pos = pos + center;
        }
    }

    bool isAir(Vector3<float> pos) {
        return !Global::getClientInstance()->getBlockSource()->getBlock(pos.floor().ToInt())->GetBlockLegacy()->isSolid();
    }

    bool isValidBlock(Vector3<int> blockPos, bool isRedstone, bool exposedOnly) {
        if (blockPos == NULL) return false;

        BlockSource* source = Global::getClientInstance()->getBlockSource();
        if (!source) return false;
        Block* block = source->getBlock(blockPos);

        //block exist check
        if (!block) return false;

        //block id check
        int blockId = block->GetBlockLegacy()->getBlockID();
        if (isRedstone) {
            if (blockId != 73 && blockId != 74) return false;

            isExposed = isRedstoneOreExposed(blockPos);
        }
        else if (block->GetBlockLegacy()->GetMaterialType() == MaterialType::Air) return false;

        //distance check
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return false;
        if (player->getStateVector()->Position.distance(blockPos.ToFloat()) >= range) return false;

        if (exposedOnly) {
            if (!isRedstoneOreExposed(blockPos)) return false;
        }

        return true;
    }

    int getBlockBreakFace(Vector3<float> bp) {
        if (isAir(bp.add(Vector3<float>(0, -1, 0)))) return 1;
        if (isAir(bp.add(Vector3<float>(0, 0, 1)))) return 2;
        if (isAir(bp.add(Vector3<float>(0, 0, -1)))) return 3;
        if (isAir(bp.add(Vector3<float>(1, 0, 0)))) return 4;
        if (isAir(bp.add(Vector3<float>(-1, 0, 0)))) return 5;
        if (isAir(bp.add(Vector3<float>(0, 1, 0)))) return 0;

        return 1;
    }

    bool isRedstoneOreExposed(Vector3<int> blockPos) {
        BlockSource* source = Global::getClientInstance()->getBlockSource();
        if (!source) return false;
        static std::vector<Vector3<int>> checklist = {
            Vector3<int>(0, -1, 0), Vector3<int>(0, 1, 0),
            Vector3<int>(0, 0, -1), Vector3<int>(0, 0, 1),
            Vector3<int>(-1, 0, 0), Vector3<int>(1, 0, 0),
        };
        for (int i = 0; i < checklist.size(); i++) {
            MaterialType mtype = source->getBlock(blockPos.add(checklist[i]))->GetBlockLegacy()->GetMaterialType();
            if (mtype == MaterialType::Air || mtype == MaterialType::ReplaceablePlant) {
                return true;
            }
        }

        return false;
    }

    void stopBreakingBlock(Vector3<int> blockPos) {
        if (blockPos == NULL) return;

        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;

        GameMode* gamemode = player->getGamemode();
        if (!gamemode || !player) return;

        BlockSource* source = Global::getClientInstance()->getBlockSource();
        if (!source) return;

        Block* block = source->getBlock(blockPos);

        if (!block) return;

        gamemode->stopDestroyBlock(blockPos);
    }

    void stopMining() {
        stopBreakingBlock(miningBlockPos);
        miningBlockPos = NULL;
        Global::miningPosition = NULL;
        isRedstoneGettingDestroyed = false;
        Global::shouldAttack = true;
        TimeUtils::resetTime("extraHealthMs");
        hasPacketSent = false;
        hasSelectedSlot = false;
        animationsTime = 0;
        ChatUtils::sendMessage("Stopped mining redstone.");
    }

    bool findBestTool(Block* block) {
        PlayerInventory* playerInventory = Global::getClientInstance()->getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        bool isAxe = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Wood;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (isAxe) {
                    if (stack->getItem()->isAxe()) {
                        if (previousSlot != i) {
                            playerInventory->hotbarSlot = i;
                            currentPacketSlot = i;
                        }
                        return true;
                    }
                }
                else {
                    if (stack->getItem()->isPickaxe()) {
                        if (previousSlot != i) {
                            playerInventory->hotbarSlot = i;
                            currentPacketSlot = i;
                        }
                        return true;
                    }
                }
            }
        }

        return false;
    }

    float calculatePercentage(float currentMs, float destroyMs) {
        return (currentMs / destroyMs) * 100.0f;
    }

    Vector3<int> findPathToBlock(Vector3<int> blockPos, int searchRange) {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) {
            return NULL;
        }

        static vector<Vector3<int>> blocks;

        if (blocks.empty()) {
            for (int x = -searchRange; x <= searchRange; x++) {
                for (int z = -searchRange; z <= searchRange; z++) {
                    for (int y = -searchRange; y <= searchRange; y++) {
                        if (abs(x) + abs(y) + abs(z) <= searchRange) blocks.push_back(Vector3<int>(x, y, z));
                    }
                }
            }
            sort(blocks.begin(), blocks.end(), [](Vector3<int> start, Vector3<int> end) {
                return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
                });
        }

        for (const Vector3<int>& offset : blocks) {
            Vector3<int> currentBlockPos = Vector3<int>(blockPos.x + offset.x, blockPos.y + offset.y, blockPos.z + offset.z);
            Block* currentBlock = Global::getClientInstance()->getBlockSource()->getBlock(currentBlockPos);
            MaterialType mtype = currentBlock->GetMaterialType();
            if (mtype != MaterialType::Stone && mtype != MaterialType::Wood && mtype != MaterialType::Dirt && mtype != MaterialType::Sand && mtype != MaterialType::StoneDecoration) continue;
            if (isRedstoneOreExposed(currentBlockPos)) return currentBlockPos;
        }
        return NULL;
    }


    void onEnabled() override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;
        TimeUtils::resetTime("extraHealthMs");
        TimeUtils::resetTime("extraHealthDelay");
        isRedstoneGettingDestroyed = false;
        shouldSetBackSlot = false;
    }

    void onEvent(ActorBaseTickEvent* event) override
    {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) {
            return;
        }

        GameMode* gamemode = player->getGamemode();
        if (!gamemode || !player)
            return;

        BlockSource* source = Global::getClientInstance()->getBlockSource();
        if (!source) return;

        if (player->getAttribute(AttributeId::Health) == nullptr) {
            return;
        }

        absorption = player->getAbsorption();

        //Avoid hive anticheat checks
        if (!TimeUtils::hasTimeElapsed("extraHealthDelay", 60, false)) {
            isRedstoneGettingDestroyed = false;
            Global::shouldAttack = true;
            return;
        }

        Vector3<int> playerBlockPos = player->getAABBShape()->PosLower.ToInt();
        PlayerInventory* supplies = player->getSupplies();
        Inventory* inventory = supplies->inventory;

        bool isItDestroyed = false;

        // Set the currentDestroyMS to destroyMs
        static float currentDestroyMs = destroyMs;

        if (shouldSetBackSlot) {
            if (previousSlot == currentPacketSlot) {
                shouldSetBackSlot = false;
            }

            if (supplies->hotbarSlot != currentPacketSlot) {
                supplies->hotbarSlot = previousSlot;
            }
        }

        //Return if extra health is full
        if (10 <= absorption) {
            stopBreakingBlock(miningBlockPos);
            if (miningBlockPos != NULL) {
                shouldSetBackSlot = true;
                miningBlockPos = NULL;
            }
            Global::miningPosition = NULL;
            isRedstoneGettingDestroyed = false;
            Global::shouldAttack = true;
            hasPacketSent = false;
            hasSelectedSlot = false;
            TimeUtils::resetTime("extraHealthMs");
            animationsTime = 0;
            return;
        }

        if (animationsTime >= 10) {
            animationsTime = 10;
        }

        if (isValidBlock(miningBlockPos, exposedOnly, true)) { // If mining
            bool isOnGround = player->isOnGround();
            if (!isOnGround) currentDestroyMs += 39.5f;
            Block* block = source->getBlock(miningBlockPos);
            if (hasPacketSent) {
                if (!hasSelectedSlot) {
                    hasSelectedSlot = true;
                }
            }
            else {
                findBestTool(block);
            }

            // Get the mining block ID
            int blockId = source->getBlock(miningBlockPos)->GetBlockLegacy()->getBlockID();

            if (blockId != 73 && blockId != 74) {
                Covered = true;
            }
            else if (blockId == 73 || blockId == 74) {
                Covered = false;
            }

            if (TimeUtils::hasTimeElapsed("extraHealthMs", currentDestroyMs, true)) {
                isRedstoneGettingDestroyed = true;
                Global::shouldAttack = false;

                if (findBestTool(block)) {
                    gamemode->destroyBlock(miningBlockPos, RedstoneSide);
                }
                gamemode->stopDestroyBlock(miningBlockPos);
                shouldSetBackSlot = true;
                TimeUtils::resetTime("extraHealthDelay");
            }
            else {
                if (isOnGround) animationsTime++;
                isRedstoneGettingDestroyed = false;
                Global::shouldAttack = true;
            }
        }
        else //find new block
        {
            stopBreakingBlock(miningBlockPos);
            miningBlockPos = NULL;
            Global::miningPosition = NULL;
            isRedstoneGettingDestroyed = false;
            Global::shouldAttack = true;
            TimeUtils::resetTime("extraHealthMs");
            previousSlot = supplies->hotbarSlot;
            hasPacketSent = false;
            hasSelectedSlot = false;
            animationsTime = 0;
            ItemStack* stack = supplies->inventory->getItem(supplies->hotbarSlot);
            if (stack != nullptr && stack->isBlockType()) return;
            static vector<Vector3<int>> blocks;
            vector<Vector3<int>> unExposedRedstones;
            vector<Vector3<int>> exposedRedstones;

            if (blocks.empty()) {
                for (int x = -range; x <= range; x++) {
                    for (int z = -range; z <= range; z++) {
                        for (int y = -range; y <= range; y++) {
                            blocks.push_back(Vector3<int>(x, y, z));
                        }
                    }
                }
                sort(blocks.begin(), blocks.end(), [](Vector3<int> start, Vector3<int> end) {
                    return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
                    });
            }

            for (const Vector3<int>& offset : blocks) {
                Vector3<int> blockPos = Vector3<int>(playerBlockPos.x + offset.x, playerBlockPos.y + offset.y, playerBlockPos.z + offset.z);

                if (isValidBlock(blockPos, true, false))
                {
                    if (isRedstoneOreExposed(blockPos)) exposedRedstones.push_back(blockPos);
                    else unExposedRedstones.push_back(blockPos);
                }
                else continue;
            }
            if (!exposedRedstones.empty()) {
                for (const Vector3<int>& blockPos : exposedRedstones) {
                    miningBlockPos = blockPos;
                    Global::miningPosition = blockPos;

                    RedstoneSide = getBlockBreakFace(miningBlockPos.ToFloat());

                    gamemode->startDestroyBlock(miningBlockPos, RedstoneSide, isItDestroyed);
                    currentDestroyMs = destroyMs; // Set the currentDestroyMs to destroyMs
                    return;
                }
            }
            else if (!exposedOnly && !unExposedRedstones.empty()) {
                for (int i = 1; i < 3; i++) {
                    for (const Vector3<int>& blockPos : unExposedRedstones) {
                        auto foundBlock = findPathToBlock(blockPos, i);
                        if (foundBlock == NULL) continue;
                        miningBlockPos = foundBlock;
                        Global::miningPosition = blockPos;

                        RedstoneSide = getBlockBreakFace(miningBlockPos.ToFloat());

                        gamemode->startDestroyBlock(miningBlockPos, RedstoneSide, isItDestroyed);
                        currentDestroyMs = blockDestroyMs; // Set the currentDestroyMs to blockDestroyMs
                        return;
                    }
                }
            }
        }
    }

    void onEvent(IntersectsTickEvent* event) override {

        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr || !isRedstoneGettingDestroyed || miningBlockPos == NULL) {
            return;
        }
        Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, miningBlockPos.ToFloat());

        if (Side == 0) {
            player->setPitch(angle.x);
            player->setXHeadRotation(angle.y);
            player->setBodyRotation(angle.y);
        }

        player->getLevel()->getHitResult()->BlockFace = RedstoneSide;
        player->getLevel()->getHitResult()->IBlockPos = miningBlockPos;
        player->getLevel()->getHitResult()->HitType = 0;
        player->getLevel()->getHitResult()->AbsoluteHitPos = miningBlockPos.ToFloat();
    } 


    void onEvent(PacketEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;

        //Spoof
        if (event->Packet->getId() == PacketID::MobEquipment) {
            auto* pkt = reinterpret_cast<MobEquipmentPacket*>(event->Packet);
            if (pkt->mSlot == previousSlot) {
                if (shouldSetBackSlot || miningBlockPos == NULL) {
                    shouldSetBackSlot = false;
                }
                else {
                    ItemStack* stack = player->getSupplies()->inventory->getItem(pkt->mSlot);
                    if (stack == nullptr || !stack->isBlockType()) {
                        *event->cancelled = true;
                    }
                    else if (miningBlockPos != NULL) {
                        stopMining();
                    }
                }
            }
            else if (pkt->mSlot == currentPacketSlot) {
                hasPacketSent = true;
            }
            else if (miningBlockPos != NULL) {
                stopMining();
            }
        }

        if (!isRedstoneGettingDestroyed || miningBlockPos == NULL) {
            return;
        }

        Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, miningBlockPos.ToFloat());

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            auto* pkt = (PlayerAuthInputPacket*)event->Packet;
            if (pkt) {
                pkt->rotation.x = angle.x;
                pkt->rotation.y = angle.y;
                pkt->headYaw = angle.y;
            }
        }
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto instance = Global::getClientInstance();
        auto player = instance->getLocalPlayer();
        if (player == nullptr || !render) { //  || !render || miningBlockPos == NULL || !instance->getMinecraftGame()->getCanUseKeys()
            return;
        }

        static EasingUtil inEase;

        (instance->getMinecraftGame()->CanUseKeys && miningBlockPos != NULL && render && !TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 200, false)) ?
            inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 10.f / 10) // Increase the animation
            : inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10); // Decrease the animation

        float inScale = inEase.easeOutExpo();

        if (inEase.isPercentageMax())
            inScale = 1;

        ImScaleStart();

        // Set a UIColor for the render background
        UIColor RenderColor = UIColor(212, 173, 48); // Yellow Color for mining

        float RenderX = instance->getGuiData()->mcResolution.x / 2; // The X Position (center)
        float RenderY = (instance->getGuiData()->mcResolution.y / 2) + 90; // The Y Position (center)

        Vector2<float> RenderPos(RenderX, RenderY);

        // The rendering text
        std::string RenderText = "Queued";

#pragma region RenderColor & RenderText
        if (!isExposed) {
            // If the Block is not a redstone ore
            if (Covered) {
                RenderColor = UIColor(193, 54, 52); // Set the color to red (covered)
                RenderText = "Covered";

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
            else {
                if (absorption >= 10) {
                    RenderColor = UIColor(201, 62, 220); // Set the color to pruple (queued)
                    RenderText = "Queued";
                }
                else {
                    RenderColor = UIColor(212, 173, 48); // Set the color to yellow (mining)
                    RenderText = "Mining";
                }

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
        }
        else {
            // If the Block is not a redstone ore
            if (Covered) {
                RenderColor = UIColor(193, 54, 52); // Set the color to red (covered)
                RenderText = "Covered";

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
            else {
                if (absorption >= 10) {
                    RenderColor = UIColor(201, 62, 220); // Set the color to pruple (queued)
                    RenderText = "Queued";
                }
                else {
                    RenderColor = UIColor(212, 173, 48); // Set the color to yellow (mining)
                    RenderText = "Mining";
                }

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
        }
#pragma endregion

        Vector2<float> TextPos(RenderX, RenderY - 5);
        float TextLength = ImRenderUtil::getTextWidth(&RenderText, 1);
        TextPos.x -= TextLength / 2;

        static float Animations = animationsTime * 14;
        Animations = Math::animate(animationsTime * 14, Animations, ImRenderUtil::getDeltaTime() * 10.f);

        // The rendering rectangle
        Vector4<float> RenderRect(RenderPos.x - 70, RenderPos.y - 8, RenderPos.x + 70, RenderPos.y + 15);
        Vector4<float> AnimatedRenderRect(RenderRect.x, RenderRect.y, (RenderRect.z - 147) + Animations, RenderRect.w);

        // Draw a gray background
        ImRenderUtil::fillRectangle(RenderRect, UIColor(33, 33, 33), 1.f, 9.f);
        ImRenderUtil::fillShadowRectangle(RenderRect, UIColor(33, 33, 33), 1, 40, 0, 9.f);

        // Draw the color of the thing regen is doing currently to the redstone from (mining, queued, covering, uncovering, jewing & covored)
        ImRenderUtil::fillRectangle(AnimatedRenderRect, RenderColor, 1.f, 9.f);
        ImRenderUtil::fillShadowRectangle(AnimatedRenderRect, RenderColor, 1, 40, 0, 9.f);

        // Draw the current thing regen is doing to the redstone from (mining, queued, covering, uncovering, jewing & covored)
        ImRenderUtil::drawText(TextPos, &RenderText, UIColor(255, 255, 255), 1, 1.f, true);

        ImScaleEnd(inScale, inScale, ImVec2(RenderRect.getCenter().x, RenderRect.getCenter().y));
        //Vector4<float> rectSmooth = Vector4<float>(RenderRect.x, rect.y, (rect.z - 120) + (cool * 4.4), rect.w);
    }

    void onDisabled() override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) {
            return;
        }
        stopBreakingBlock(miningBlockPos);
        if (miningBlockPos != NULL) {
            player->getSupplies()->hotbarSlot = currentPacketSlot;
        }
        Global::shouldAttack = true;
    }

    std::string getModeName() override {
        return std::string("");
    }
};
