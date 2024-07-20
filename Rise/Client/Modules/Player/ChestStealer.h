#pragma once

class ChestStealer : public Module {
public:
    ChestStealer(int keybind = 7, bool enabled = true) :
        Module("ChestStealer", "Player", "Steal items out of chests", keybind, enabled) {
        addSlider("SPS", "How many items are moved a second", &SPS, 1, 20);
    }

    float SPS = 20;

    int item = 0;
    int maxItem = 56;

    void onEvent(ContainerTickEvent* event) override {
        if (TimeUtils::hasTimeElapsed("Cs", 500, false))
            item = 0;

        ContainerScreenController* controller = event->Controller;

        if (!controller)
            return;

        bool itemsRemaining = true;

        if (TimeUtils::hasTimeElapsed("Cs", 1000 / SPS, true)) {
            for (int i = 0; i < 56; ++i) {
                if (controller->_getItemStack(Containers::Container, i)->item != nullptr) {
                    controller->shiftItems(Containers::Container, i);
                    itemsRemaining = false;
                    break;
                }
            }
        }

        // Close the container if no items are remaining
        if (!itemsRemaining) {
            controller->closeContainer();
        }
    }

    std::string getModeName() override {
        return "Delayed";
    }
};