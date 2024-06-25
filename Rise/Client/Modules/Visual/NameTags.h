#pragma once

class NameTags : public Module
{
public:
    NameTags(int keybind = 7, bool enabled = true) :
        Module("NameTags", "Visual", "Displays peoples names through walls", keybind, enabled)
    {
        addSlider("Range", "The range entities has to be on to render", &range, 1, 200);
        addBool("Self", "Renders the esp on localplayer", &selfrender);
        addSlider("Opacity", "The opacity of the arraylist", &opacity, 0, 100);
        addSlider("Rounding", "The opacity of the arraylist", &rounding, 0, 100);
        addBool("Shadow", "Render shadows for the rectangle", &shadow);
        addBool("Fill Shadow", "Fill the rectangle with shadow", &shadowfilled);
        addSlider("Shadow strenght", "The strenght of the shadow", &shadowstrenght, 0, 200);
    }

    float range = 70;
    bool selfrender = true;
    bool shadow = true;
    bool shadowfilled = true;
    float shadowstrenght = 150;
    float opacity = 50;
    float rounding = 0;


    void drawNametags(Actor* entity)
    {
        Player* player = Global::getClientInstance()->getLocalPlayer();

        if (!entity || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
            return;

        AABBShapeComponent* shape = entity->getAABBShape();
        if (!shape || IsBadReadPtr(shape, sizeof(AABBShapeComponent))) return;

        Vector3<float> renderPos = entity->getRenderPosition().submissive(Vector3<float>(0, -1.f, 0));

        float distance = renderPos.distance(Global::getClientInstance()->getLevelRender()->getOrigin());

        float textSize = fmax(0.8f, 1.35f / distance);

        Vector2<float> output1;
        if (!Global::getClientInstance()->WorldToScreen(renderPos, output1)) return;

        std::string user(entity->getNametag()->c_str());
        user = Utils::sanitize(user);
        user = user.substr(0, user.find('\n'));

        float textWidth = ImRenderUtil::getTextWidth(&user, textSize);
        float textHeight = ImRenderUtil::getTextHeight(textSize);
        Vector2<float> textPos = Vector2<float>(output1.submissive(Vector2<float>(textWidth / 2, 0)));

        Vector4<float> rectPos;
        rectPos.x = textPos.x - 3.f * textSize;
        rectPos.y = textPos.y - 1.f * textSize;
        rectPos.z = textPos.x + textWidth + 3.f * textSize;
        rectPos.w = textPos.y + textHeight + 1.f * textSize;

        ImRenderUtil::fillRectangle(rectPos, UIColor(0, 0, 0), opacity / 100, rounding / 10);
        if (shadow)
        ImRenderUtil::fillShadowRectangle(rectPos, UIColor(0, 0, 0), (opacity / 100) * 2, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground);

        ImRenderUtil::drawText(output1.submissive(Vector2<float>(textWidth / 2, 0)), &user, UIColor(255, 255, 255), textSize, 1, true);
    }


    void onEvent(ImGuiRenderEvent* event) override
    {
        Player* player = Global::getClientInstance()->getLocalPlayer();
        if (!player || !Global::getClientInstance()->getMinecraftGame()->getCanUseKeys())
            return;

        //if (selfrender) {
            //drawSelfESP();
        //}

        auto list = Global::getClientInstance()->getLocalPlayer()->getLevel()->getRuntimeActorList();

        for (auto* actor : list) {
            if (actor->isAlive() && !actor->isBot()) {
                drawNametags(actor);
            }
        }
    }
};
