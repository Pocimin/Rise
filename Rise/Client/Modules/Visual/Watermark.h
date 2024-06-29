#pragma once

class Watermark : public Module
{
public:
    Watermark(int keybind = Keys::NONE, bool enabled = true) :
        Module("Watermark", "Visual", "Displays the client's watermark", keybind, enabled)
    {
        addEnum("Style", "The style of the watermark", { "Simple", "Aeolus" }, &Style);
        addSlider("Opacity", "The opacity of the rectangle", &opacity, 0, 1);
        addBool("Shadow", "Render shadows for the rectangle", &shadow);
        addBool("Fill Shadow", "Fill the rectangle with shadow", &shadowfilled);
        addSlider("Shadow strenght", "The strenght of the shadow", &shadowstrenght, 0, 200);
    }

    float opacity = 0.2;
    bool shadow = true;
    bool shadowfilled = true;
    float shadowstrenght = 150;
    int Style = 0;

    void onEvent(ImGuiRenderEvent* event) override {
        if (Style == 0) {
            Vector2<float> pos2 = Vector2<float>(0.f, 0.f);

            int ind = 0;

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            std::string name = xorstr_("Rise");
            std::string version = xorstr_("Rebuild");

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 46), &version, UIColor(44, 44, 44), 1.1, 1, true);
            ImGui::PopFont();

            UIColor customColor = UIColor(129, 221, 224);  // Define the custom color

            for (char c : (std::string)name)
            {
                std::string string = Utils::combine(c, "");

                float charWidth = ImRenderUtil::getTextWidth(&string, 2.3);
                float charHeight = ImRenderUtil::getTextHeight(2.3);

                ImRenderUtil::drawShadowSquare(Vector2<float>(pos2.x + charWidth / 2, pos2.y + charHeight / 1.2), 15.f, customColor, 0.85f, 70.f, 0);
                ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, customColor, 2.3, 1, true);

                pos2.x += charWidth;
                ++ind;
            }
            ImGui::PopFont();
        }
        else if (Style == 1) {

            std::string name = xorstr_("Rise - Rebuild");

            Vector2<float> textPos = Vector2<float>(15.f, 15.f);

            float verlen = ImRenderUtil::getTextWidth(&name, 1.16) + 22;
            float height = ImRenderUtil::getTextHeight(1.16) + 18;

            Vector4<float> boxRect = Vector4<float>(textPos.x - 7.5, textPos.y - 3.5, verlen, height);

            ImRenderUtil::fillRectangle(boxRect, UIColor(0, 0, 0), opacity);
            if (shadow)
                ImRenderUtil::fillShadowRectangle(boxRect, UIColor(0, 0, 0), 1, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground);

            ImRenderUtil::drawText(textPos, &name, UIColor(255, 255, 255), 1.16, 1, true);

        };
    }
};