#pragma once
#include "../../Base/SDK/Classes/Render/MinecraftUIRenderContext.h"

enum Type
{
	Internal = 0, // UserPackage
	External = 1, // Raw
};

class RenderUtils
{
public:
	static inline MinecraftUIRenderContext* renderContext = nullptr;

	static void Initialize(MinecraftUIRenderContext* RenderCtx) {
		static bool inited = false;
		if (inited) return;
		renderContext = RenderCtx;
		if (renderContext != nullptr) inited = true;
	};

	inline static void fillRectangle(Vector4<float> pos, UIColor col, float alpha) {
		renderContext->fillRectangle(Vector4<float>(pos.x, pos.z, pos.y, pos.w), col, alpha);
	}

	inline static void drawRectangle(Vector4<float> pos, UIColor col, float alpha, float width) {
		renderContext->drawRectangle(Vector4<float>(pos.x, pos.z, pos.y, pos.w), col, alpha, width);
	}

	static void fillGradientRectangle(const Vector4<float>& rect, float alpha, int quality, int index) {
		int i = 0;
		for (float x = rect.x; x < rect.z; x += quality) {
			fillRectangle(Vector4<float>(x, rect.y, x + quality, rect.w), ColorUtils::RainbowDark(3, i * index), alpha);
			i++;
		}
	}

	static void drawText(FontRepos* font, Vector2<float> pos, std::string* str, UIColor color, float size, float alpha, bool shadow)
	{
		static CaretMeasureData black = CaretMeasureData(20, false);

		float tPos[4] = { pos.x, pos.x + 1000, pos.y, pos.y + 1000 };
		TextMeasureData data = TextMeasureData(size, shadow);

		renderContext->drawText(font, tPos, str, color.arr, alpha, 0, &data, &black);
	}

	static void renderImage(std::string filePath, Vector2<float> ImagePos, Vector2<float> ImageDimension, Type type = External)
	{
		TexturePtr* texturePtr = new TexturePtr();
		renderContext->getTexture(texturePtr, new ResourceLocation(type, filePath));
		renderContext->drawImage(texturePtr, &ImagePos, &ImageDimension, Vector2<float>(0.f, 0.f), Vector2<float>(1.f, 1.f));
	}

	static void flushImage(UIColor color = UIColor(255, 255, 255), float alpha = 1.f)
	{
		static StringHasher flushString = StringHasher(0xA99285D21E94FC80, "ui_flush");
		renderContext->flushImages(color, alpha, flushString);
	}
};