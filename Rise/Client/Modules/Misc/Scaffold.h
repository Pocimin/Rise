#pragma once

class Scaffold : public Module
{
public:
    Scaffold(int keybind = 82, bool enabled = false) :
        Module("Scaffold", "Misc", "Builds blocks for you.", keybind, enabled)
    {
        addEnum("Rotations", "The rotation mode", { "Normal", "Moonwalk" }, &rotations);
        addEnum("Side", "The side for the rotations", { "Client", "Server" }, &Side);
        addEnum("Switch", "How to hold the block", { "Hold", "Spoof", "ClientSpoof" }, &holdStyle);
        addEnum("Swing", "The swing style for scaffold", { "Normal", "None" }, &Global::SwingType);
        addEnum("TowerMode", "The tower mode", { "Jump", "Velocity" }, &towerMode);
        addEnum("Placement", "The placement mode", { "Normal", "Flareon", "Legit" }, &placement);
        addBool("Keep Y", "Kepps your y level when placing blocks", &Ylock);
        addSlider("PlacesPerTick", "how many blocks it can place in tick", &placesPerTick, 1, 10);
        addSlider("Distance", "The place distance (how far to place blocks)", &range, 3, 8);
        addSlider("Extend", "Distance your position to target block", &extend, 0, 10);
    }

private:
    //Settings
    int rotations = 0;
    int Side = 0;
    int holdStyle = 0;
    int towerMode = 0;
    int placement = 0;
    float extend = 0;
    float range = 5.f;
    float placesPerTick = 1.f;
    bool Ylock = false;

    //Other
    int prevSlot = 0;
    int currentPacketSlot = -1;
    bool packetSent = false;
    int placesPerTickCount = 0;
    float lockedY = 0;
    bool isHoldingSpace = false;
    bool foundBlock = false;
    Vector3<float> BlockPos;
    Vector3<float> ClickBlockPos;
    Vector3<float> BlockBelow;
public:

    bool canPlace(Vector3<float> pos) {
        return Global::getClientInstance()->getBlockSource()->getBlock(pos.floor().ToInt())->GetMaterialType() == MaterialType::Air;
    }

    bool findBlock() {
        PlayerInventory* playerInventory = Global::getClientInstance()->getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;
        int slot = previousSlot;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
                    if (previousSlot != i) {
                        playerInventory->hotbarSlot = i;
                        if (holdStyle == 2 && currentPacketSlot != i) {
                            currentPacketSlot = i;
                            packetSent = false;
                        }
                    }
                    return true;
                }
            }
        }

        return false; // false if we are using checks
    }

    Vector3<float> getBlockBelow(Player* player) {
        Vector3<float> blockBelow = player->getAABBShape()->PosLower;
        blockBelow.y -= 1.f;
        return blockBelow;
    }

    void adjustYCoordinate(const Vector3<float>& blockBelowReal) {
        if (floor(blockBelowReal.y) < floor(lockedY)) {
            lockedY = blockBelowReal.y + Global::getClientInstance()->getLocalPlayer()->getStateVector()->Velocity.y;
        }
    }

    Vector3<float> getExtendedPosition(const Vector3<float>& velocity, Vector3<float>& blockBelow, float extendValue) {
        Vector3<float> extendedBlock = blockBelow;
        Vector3<float> normalizedVelocity = velocity.Normalize();
        extendedBlock.x += normalizedVelocity.x * extendValue;
        extendedBlock.z += normalizedVelocity.z * extendValue;
        return extendedBlock;
    }

    Vector3<float> getNextBlock(const Vector3<float>& velocity, const Vector3<float>& blockBelow, float extendValue) {
        Vector3<float> nextBlock = blockBelow;
        if (abs(velocity.x) > abs(velocity.z)) {
            nextBlock.x += (velocity.x > 0 ? 1 : (velocity.x < 0 ? -1 : 0)) * extendValue;
        }
        else {
            nextBlock.z += (velocity.z > 0 ? 1 : (velocity.z < 0 ? -1 : 0)) * extendValue;
        }
        return nextBlock;
    }
    Vector2<float> getNormAngle(const Vector2<float>& angle) {
        float x = angle.x;
        float y = angle.y;
        while (x > 90.f)
            x -= 180.0f;
        while (x < -90.f)
            x += 180.0f;

        while (y > 180.0f)
            y -= 360.0f;
        while (y < -180.0f)
            y += 360.0f;
        return Vector2<float>(x, y);
    }

    bool buildBlock(Vector3<float> blockBelow) {
        BlockPos = Vector3<float>(blockBelow.ToInt().ToFloat().add(Vector3<float>(0.5f, 0, 0.5f)));
        Vector3<float> vel = Global::getClientInstance()->getLocalPlayer()->getStateVector()->Velocity;
        vel = vel.Normalize();  // Only use values from 0 - 1
        blockBelow = blockBelow.floor();

        if (Global::getClientInstance()->getBlockSource()->getBlock(Vector3<int>(blockBelow.ToInt()))->GetBlockLegacy()->getBlockID() == 0) {
            Vector3<int> blok(blockBelow.ToInt());

            // Find neighbour
            static std::vector<Vector3<int>*> checklist;
            if (checklist.empty()) {
                checklist.push_back(new Vector3<int>(0, -1, 0));
                checklist.push_back(new Vector3<int>(0, 1, 0));

                checklist.push_back(new Vector3<int>(0, 0, -1));
                checklist.push_back(new Vector3<int>(0, 0, 1));

                checklist.push_back(new Vector3<int>(-1, 0, 0));
                checklist.push_back(new Vector3<int>(1, 0, 0));
            }

            bool foundCandidate = false;
            int i = 0;
            for (auto current : checklist) {
                Vector3<int> calc = blok.submissive(*current);
                if (Global::getClientInstance()->getBlockSource()->getBlock(calc)->GetBlockLegacy()->getBlockID() != 0) {
                    // Found a solid block to click
                    foundCandidate = true;
                    blok = calc;
                    break;
                }
                i++;
            }

            ClickBlockPos = blok.ToFloat().add(Vector3<float>(0.5f, 0, 0.5f));
            if (foundCandidate) {
                if (1 <= placement) {
                    auto player = Global::getClientInstance()->getLocalPlayer();
                    player->getLevel()->getHitResult()->BlockFace = i;
                    player->getLevel()->getHitResult()->IBlockPos = BlockPos.ToInt();
                    player->getLevel()->getHitResult()->HitType = 0;
                    player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos.ToFloat();
                }
                Global::getClientInstance()->getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
                return true;
            }
        }
        return false;
    }

    bool predictBlock(Vector3<float> blockBelow) {
        Vector3<float> vel = Global::getClientInstance()->getLocalPlayer()->getStateVector()->Velocity;
        vel = vel.Normalize();  // Only use values from 0 - 1
        blockBelow = blockBelow.floor();

        static std::vector<Vector3<int>> checkBlocks;
        if (checkBlocks.empty()) {  // Only re sort if its empty
            for (int y = -range; y < range; y++) {
                for (int x = -range; x < range; x++) {
                    for (int z = -range; z < range; z++) {
                        checkBlocks.push_back(Vector3<int>(x, y, z));
                    }
                }
            }
            // https://www.mathsisfun.com/geometry/pythagoras-3d.html c2 = x2 + y2 + z2 funny
            std::sort(checkBlocks.begin(), checkBlocks.end(), [](Vector3<int> first, Vector3<int> last) {
                return sqrtf((float)(first.x * first.x) + (float)(first.y * first.y) + (float)(first.z * first.z)) < sqrtf((float)(last.x * last.x) + (float)(last.y * last.y) + (float)(last.z * last.z));
                });
        }

        for (const Vector3<int>& blockOffset : checkBlocks) {
            Vector3<int> currentBlock = Vector3<int>(blockBelow.ToInt()).add(blockOffset);
            BlockPos = Vector3<float>(currentBlock.ToInt().ToFloat().add(Vector3<float>(0.5f, 0, 0.5f)));

            // Normal tryScaffold after it sorts
            if (Global::getClientInstance()->getBlockSource()->getBlock(Vector3<int>(currentBlock))->GetBlockLegacy()->getBlockID() == 0) {
                Vector3<int> blok(currentBlock);

                // Find neighbour
                static std::vector<Vector3<int>*> checklist;
                if (checklist.empty()) {
                    checklist.push_back(new Vector3<int>(0, -1, 0));
                    checklist.push_back(new Vector3<int>(0, 1, 0));

                    checklist.push_back(new Vector3<int>(0, 0, -1));
                    checklist.push_back(new Vector3<int>(0, 0, 1));

                    checklist.push_back(new Vector3<int>(-1, 0, 0));
                    checklist.push_back(new Vector3<int>(1, 0, 0));
                }

                bool foundCandidate = false;
                int i = 0;
                for (auto current : checklist) {
                    Vector3<int> calc = blok.submissive(*current);
                    //bool Y = ((region->getBlock(calc)->blockLegacy))->material->isReplaceable;
                    if (Global::getClientInstance()->getBlockSource()->getBlock(calc)->GetBlockLegacy()->getBlockID() != 0) {
                        // Found a solid block to click
                        foundCandidate = true;
                        blok = calc;
                        break;
                    }
                    i++;
                }
                ClickBlockPos = blok.ToFloat().add(Vector3<float>(0.5f, 0, 0.5f));
                if (foundCandidate) {
                    if (1 <= placement) {
                        auto player = Global::getClientInstance()->getLocalPlayer();
                        player->getLevel()->getHitResult()->BlockFace = i;
                        player->getLevel()->getHitResult()->IBlockPos = BlockPos.ToInt();
                        player->getLevel()->getHitResult()->HitType = 0;
                        player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos;
                    }
                    Global::getClientInstance()->getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
                    return true;
                }
            }
        }
        return false;
    }

    void onEnabled() override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;

        Vector3<float> blockbelow = getBlockBelow(player);

        lockedY = blockbelow.y;
        prevSlot = Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot;
        currentPacketSlot = -1;

        BlockBelow = blockbelow;

        placesPerTickCount = 0;
    }

    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;

        placesPerTickCount = 0;

        foundBlock = findBlock();
        if (!foundBlock) return;

        float speed = Global::getClientInstance()->getLocalPlayer()->getStateVector()->Velocity.magnitudexz();
        Vector3<float> velocity = Global::getClientInstance()->getLocalPlayer()->getStateVector()->Velocity.Normalize();
        Vector3<float> blockBelow = getBlockBelow(player);
        BlockBelow = blockBelow;
        Vector3<float> yLockedBlockBelow = Vector3<float>(blockBelow.x, lockedY, blockBelow.z);
        if (Ylock) blockBelow = yLockedBlockBelow;

        //Horizontal
        if (Ylock) {
            adjustYCoordinate(getBlockBelow(player));
            blockBelow = yLockedBlockBelow;
        }
        else lockedY = blockBelow.y;
        for (int i = 0; i < placesPerTick; i++) {
            //Place block below
            if (canPlace(blockBelow)) {
                if (predictBlock(blockBelow)) {
                    // Placed
                }
            }
            //Extend
            if (!isHoldingSpace) {
                if (placement == 1) {
                    for (int i = 1; i < extend; i++) {
                        Vector3<float> nextBlock = getNextBlock(velocity, blockBelow, i);
                        if (canPlace(nextBlock) && predictBlock(nextBlock)) {
                            // Placed
                            break;
                        }
                    }
                }
                else {
                    for (int i = 1; i < extend; i++) {
                        Vector3<float> nextBlock = getExtendedPosition(velocity, blockBelow, i);
                        if (canPlace(nextBlock) && predictBlock(nextBlock)) {
                            // Placed
                            break;
                        }
                    }
                }
            }
        }

        //Vertical
        if (Global::Keymap[VK_SPACE] && speed < 0.05f) {
            Vector3<int> pos = Global::getClientInstance()->getLocalPlayer()->getStateVector()->Position.ToInt();
            lockedY = getBlockBelow(player).y;
            if (towerMode == 1) player->getStateVector()->Velocity = Vector3<float>(0.f, 0.8f, 0.f);
            isHoldingSpace = true;
        }
        else {
            if (isHoldingSpace && towerMode == 1) player->getStateVector()->Velocity.y = 0.f;
            isHoldingSpace = false;
        }

        //Spoof
        if (holdStyle >= 1) {
            if (holdStyle == 2 && !packetSent) {
                packetSent = true;
                return;
            }
            player->getSupplies()->hotbarSlot = prevSlot;
        }
    }

    void onEvent(ImGuiRenderEvent* event) override {
        if (foundBlock) {
            Player* player = Global::getClientInstance()->getLocalPlayer();

            if (player == nullptr)
                return;

            GuiData* guidata = Global::getClientInstance()->getGuiData();
            float renderx = guidata->mcResolution.x / 2;
            float rendery = (guidata->mcResolution.y / 2) + 110;
            Vector4<float> plusRect = Vector4<float>(renderx, rendery + 6, 55 + renderx, rendery + 16);
            Vector2<float> textPos = Vector2<float>(renderx, rendery - 30.f);

            auto supplies = player->getSupplies();
            auto inv = supplies->inventory;

            int totalCount = 0;

            for (int i = 0; i < 36; i++) {
                ItemStack* stack = inv->getItem(i);
                if (stack->item != nullptr) {
                    if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
                        totalCount += (int)(*(uint8_t*)(((uintptr_t)stack) + 0x22));
                    }
                }
            }

            string text = "Blocks Left: " + to_string(totalCount);

            float textLen = ImRenderUtil::getTextWidth(&text, 1.2) / 2;
            textPos.x -= textLen;
            plusRect.x -= textLen;
            Vector4<float> rectpos = Vector4<float>(textPos.x - 5, textPos.y - 5, textPos.x + textLen + 17, textPos.y + 30);

            //ImRenderUtil::fillRectangle(rectpos, UIColor(0, 0, 0), 0.6f, 0.f);
            ImRenderUtil::drawText(textPos, &text, UIColor(255, 255, 255), 1.1f, 1.f, true);
        }
    }

    void onEvent(PacketEvent* event) {
        auto player = Global::getClientInstance()->getLocalPlayer();

        if (!player || !player->getStateVector() || Global::isAttacking) { return; }

        float speed = player->getStateVector()->Velocity.magnitudexz();
        Vector3<float> blockBelow = getBlockBelow(player);

        Vector2<float> angleNormal = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockPos.ToFloat());
        Vector2<float> angleMoonwalk = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockBelow.ToFloat());

        //Spoof
        if (event->Packet->getId() == PacketID::MobEquipment && holdStyle == 2) {
            if (player->getSupplies()->hotbarSlot == prevSlot) {
                *event->cancelled = true;
            }
        }

        switch (rotations)
        {
        case 0: //Normal
            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (pkt) {
                    pkt->rotation.x = angleNormal.x;
                    pkt->rotation.y = angleNormal.y;
                    pkt->headYaw = angleNormal.y;
                }
            }
            break;
        case 1: //Moonwalk
            angleMoonwalk = getNormAngle(angleMoonwalk);

            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (pkt) {
                    pkt->rotation.x = angleMoonwalk.x;
                    pkt->rotation.y = angleMoonwalk.y;
                    pkt->headYaw = angleMoonwalk.y;
                }
            }
            break;
        }
    }

    void onEvent(IntersectsTickEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        auto gm = player->getGamemode();

        if (!player || !player->getStateVector() || Global::isAttacking || Side != 0) { return; }

        Vector2<float> angleNormal = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockPos.ToFloat());
        Vector2<float> angleMoonwalk = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockBelow.ToFloat());

        switch (rotations)
        {
        case 0: //Normal

            player->setPitch(angleNormal.x);
            player->setXHeadRotation(angleNormal.y);
            player->setBodyRotation(angleNormal.y);
            break;
        case 1: //Moonwalk
            angleMoonwalk = getNormAngle(angleMoonwalk);
            player->setPitch(angleMoonwalk.x);
            player->setXHeadRotation(angleMoonwalk.y);
            player->setBodyRotation(angleMoonwalk.y);
            break;
        }
    }



    void onDisabled() override {
        if (!Global::getClientInstance() || !Global::getClientInstance()->getLocalPlayer() || !Global::getClientInstance()->getTimerClass())
            return;

        Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot = prevSlot;
    }

    std::string getModeName() override {
        return "";
    }
};
