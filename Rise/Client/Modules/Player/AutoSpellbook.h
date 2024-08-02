#pragma once

class AutoSpellbook : public Module
{
public:
    AutoSpellbook(int keybind = Keys::NONE, bool enabled = false) :
        Module("AutoSpellbook", "Player", "Automatically uses spellbook", keybind, enabled)
    {

    }

    int delay;

    void onEvent(ActorBaseTickEvent* event) {
        // when low on health we don't have this yet
        auto localPlayer = Global::getClientInstance()->getLocalPlayer();

        if (!localPlayer || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) return;

        PlayerInventory* playerInventory = localPlayer->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        if (localPlayer->getHealth() > 12) {
            return; // min health for spellbook is 16
        }

        for (int n = 0; n < 36; n++) {
            ItemStack* stack = inventory->getItem(n);
            if (stack->item != nullptr) {
                std::string ItemName = stack->getItem()->name;
                if (stack->customNameContains("Spell of Life")) {
                    if (previousSlot != n) {
                        playerInventory->hotbarSlot = n;
                        localPlayer->getGamemode()->baseUseItem(*stack);
                        playerInventory->hotbarSlot = previousSlot;
                    }
                }
            }
        }
    }
};