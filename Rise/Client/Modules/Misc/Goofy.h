#pragma once

class Goofy : public Module
{
public:
	Goofy(int keybind = Keys::NUM_0, bool enabled = false) :
		Module("Fat ngga", "Misc", "Kicks and does the funny to players", keybind, enabled)
	{
		addSlider("Range", "Range for AutoPush", &range, 1, 10);
		addSlider("Prediction", "Strength of prediction", &prediction, 0, 10);
		addBool("Advanced", "More accurate prediction", &advanced);
		addBool("Teams", "Avoid targetting team member", &teams);
	}

private:
	float range = 1.5;
	float prediction = 1.f;

	int HotbarSlotPrev = -1;
	bool advanced = false;
	bool teams = true;

	Vector3<float> previousPosition = NULL;

public:
	void onEnabled() override {
		auto player = Global::getClientInstance()->getLocalPlayer();
		if (player == nullptr) return;

		HotbarSlotPrev = Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot; // This for us to set our slot to the current one

		previousPosition = NULL;
	}

	Vector3<int> GetBlockPosition(Actor* target)
	{
		Vector3<float> pos = target->getAABBShape()->PosLower;

		return { (int)floor(pos.x), (int)floor(pos.y), (int)floor(pos.z) };
	}

	std::vector<Actor*> targetList;
	void updateTargetList() {
		auto instance = Global::getClientInstance();
		auto localPlayer = instance->getLocalPlayer();
		auto list = localPlayer->getLevel()->getRuntimeActorList();
		auto lpPos = localPlayer->getStateVector()->Position;
		for (Actor* actor : list) {
			if (actor == localPlayer || !actor->isAlive() || actor->isBot() || (teams && actor->isTeammate(localPlayer))) continue;
			float dist = lpPos.distance(actor->getStateVector()->Position);
			if (dist <= range) {
				targetList.push_back(actor);
			}
		}
	}

	//Place
	bool canPlace(Vector3<float> pos) {
		return Global::getClientInstance()->getBlockSource()->getBlock(pos.floor().ToInt())->GetMaterialType() == MaterialType::Air;
	}

	bool findBlock() {
		PlayerInventory* playerInventory = Global::getClientInstance()->getLocalPlayer()->getSupplies();
		Inventory* inventory = playerInventory->inventory;
		auto previousSlot = playerInventory->hotbarSlot;

		for (int i = 0; i < 36; i++) {
			ItemStack* stack = inventory->getItem(i);
			if (stack->item != nullptr) {
				if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
					if (previousSlot != i)
						playerInventory->hotbarSlot = i;
					return true;
				}
			}
		}
		return false;
	}

	bool buildBlock(Vector3<float> blockBelow) {
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
			Vector3<int> BlockPos = Vector3<int>(blockBelow.ToInt());
			if (foundCandidate) {
				auto player = Global::getClientInstance()->getLocalPlayer();
				player->getLevel()->getHitResult()->BlockFace = i;
				player->getLevel()->getHitResult()->IBlockPos = BlockPos;
				player->getLevel()->getHitResult()->HitType = 0;
				player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos.ToFloat();
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
			for (int y = -5; y <= 5; y++) {
				for (int x = -5; x <= 5; x++) {
					for (int z = -5; z <= 5; z++) {
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
				Vector3<int> BlockPos = Vector3<int>(currentBlock);
				if (foundCandidate) {
					auto player = Global::getClientInstance()->getLocalPlayer();
					player->getLevel()->getHitResult()->BlockFace = i;
					player->getLevel()->getHitResult()->IBlockPos = BlockPos;
					player->getLevel()->getHitResult()->HitType = 0;
					player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos.ToFloat();
					Global::getClientInstance()->getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
					return true;
				}
			}
		}
		return false;
	}

	bool buildBlockHive(Vector3<float> blockBelow) {
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
			Vector3<int> BlockPos = Vector3<int>(blockBelow.ToInt());
			if (foundCandidate && i == 1) {
				auto player = Global::getClientInstance()->getLocalPlayer();
				player->getLevel()->getHitResult()->BlockFace = i;
				player->getLevel()->getHitResult()->IBlockPos = BlockPos;
				player->getLevel()->getHitResult()->HitType = 0;
				player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos.ToFloat();
				Global::getClientInstance()->getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
				return true;
			}
		}
		return false;
	}

	void onEvent(ActorBaseTickEvent* event) override
	{
		static bool placedLastTick = false;
		targetList.clear();
		auto player = Global::getClientInstance()->getLocalPlayer();
		if (player == nullptr || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) {
			return;
		}

		GameMode* gamemode = player->getGamemode();

		if (!gamemode || !player)
			return;

		PlayerInventory* supplies = player->getSupplies();

		Vector3<int> playerBlockPos = player->getStateVector()->Position.ToInt();

		if (placedLastTick) {
			supplies->hotbarSlot = HotbarSlotPrev;
			placedLastTick = false;
		}
		else HotbarSlotPrev = Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot;

		updateTargetList();
		if (!targetList.empty()) {
			float calcYaw = (targetList[0]->getMovementInterpolator()->Rotations.y + 90) * (PI / 180);
			float c = cos(calcYaw);
			float s = sin(calcYaw);
			Vector3<float> targetPos = targetList[0]->getAABBShape()->PosLower;
			Vector3<float> predictTargetPos;
			Vector3<float> predictVector;
			Vector3<float> calcVelocity = Vector3<float>(targetPos.x - previousPosition.x, 0.f, targetPos.z - previousPosition.z);
			//Advanced
			if (advanced && previousPosition != NULL && calcVelocity.magnitudexz() != 0.f) {
				predictVector = calcVelocity.Normalize();
				predictVector.x *= prediction;
				predictVector.z *= prediction;
			}
			else {
				predictVector = Vector3<float>(c * prediction, 0.f, s * prediction);
			}
			predictTargetPos = targetPos.add(predictVector);

			static std::vector<Vector3<int>> posLists = {
				Vector3<int>(0, 0, 0),
				Vector3<int>(0, 1, 0),
			};
			for (int i = 0; i < posLists.size(); i++) {
				Vector3<float> placePosition = predictTargetPos.add(posLists[i].ToFloat());
				if (canPlace(placePosition)) {
					if (!findBlock()) return;
					if (buildBlockHive(placePosition)) {
						placedLastTick = true;
					}
					else supplies->hotbarSlot = HotbarSlotPrev;
				}
				else continue;
			}
			previousPosition = targetPos;
		}
		else previousPosition = NULL;
	}

	void onDisabled() override {
		if (!Global::getClientInstance()->getLocalPlayer()) {
			return;
		}

		Global::getClientInstance()->getLocalPlayer()->getSupplies()->hotbarSlot = HotbarSlotPrev;
	}
};
