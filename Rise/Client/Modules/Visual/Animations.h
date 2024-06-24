#pragma once

class Animations : public Module
{
public:
    Animations(int keybind = 7, bool enabled = true) :
        Module("Animations", "Visual", "Animations like its flux", keybind, enabled)
    {
        addEnum("Swing", "The Swing type", { "Flux", "Tap" }, &swingType);
        addSlider("Swing Speed", "The swing speed", &Global::swingSpeed, 1, 70);
    }

    int swingType = 0;

    void* fluxSwingAddr = (void*)(Utils::findSig(xorstr_("E8 ? ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? ? ? ? 48 8B ? F3 0F ? ? 48 8B")));
    void* tapAddr = (void*)(Utils::findSig(xorstr_("F3 0F 51 F0 0F 28 C8")));

    char ogFlux[5] = {};

    //void* punchAddr = (void*)(Utils::findSig(xorstr_("48 89 43 ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 cc cc cc cc 48 89 5c 24 ? 57 48 83 ec ? 48 8b 42 ? 48 8b da 0f 29 74 24 ? 48 8b f9 0f 29 7c 24 ? 0f 57 ff 48 85 c0 74 ? f3 0f 10 70")));
    //char ogPunch[4] = {};

    void onEvent(RenderContextEvent* event) override {
        if (!Global::getClientInstance())
            return;
    }

    void onEvent(ViewBobbingTickEvent* event) override
    {
        glm::mat4& matrix = *event->Matrix;

        if (swingType == 0) {
            //Utils::nopBytes(fluxSwingAddr, 5);
            //Utils::patchBytes((BYTE*)((uintptr_t)tapAddr), (BYTE*)"\xF3\x0F\x51\xF0", 4);
        }
        if (swingType == 1) {
            //Utils::nopBytes((BYTE*)tapAddr, 4);
            //zUtils::patchBytes((BYTE*)((uintptr_t)fluxSwingAddr), (BYTE*)"\xF3\x0F\x51\xF0", 6);
        }

        if ((GetAsyncKeyState(VK_RBUTTON) || Global::ShouldBlock) && Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) // RenderUtil::GetCTX()->ClientInstance->mcGame->CanUseKeys
        {
            matrix = glm::translate<float>(matrix, glm::vec3(0.42222223281, 0.0, -0.16666666269302368));
            matrix = glm::translate<float>(matrix, glm::vec3(-0.1f, 0.15f, -0.2f));
            matrix = glm::translate<float>(matrix, glm::vec3(-0.24F, 0.25f, -0.20F));
            matrix = glm::rotate<float>(matrix, -1.98F, glm::vec3(0.0F, 1.0F, 0.0F));
            matrix = glm::rotate<float>(matrix, 1.30F, glm::vec3(4.0F, 0.0F, 0.0F));
            matrix = glm::rotate<float>(matrix, 60.0F, glm::vec3(0.0F, 1.0F, 0.0F));
        }
        else {
            //Utils::patchBytes((BYTE*)((uintptr_t)tapSwing), (BYTE*)"\xF3\x0F\x51\xF0", 4);
        }
    }

    void onDisabled() override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getTimerClass())
            return;
    }

    std::string getModeName() override {
        return " " + std::string("Flux");
    }
};