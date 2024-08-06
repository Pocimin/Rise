#pragma once

static inline std::map<std::string, ImColor> colors =
{
	{"0", {0, 0, 0}},
	{"1", {0, 0, 0xAA}},
	{"2", {0, 0xAA, 0}},
	{"3", {0, 0xAA, 0xAA}},
	{"4", {0xAA, 0, 0}},
	{"5", {0xAA, 0, 0xAA}},
	{"6", {0xFF, 0xAA, 0}},
	{"7", {0xAA, 0xAA, 0xAA}},
	{"8", {0x54, 0x54, 0x54}},
	{"9", {0x54, 0x54, 0xff}},
	{"a", {0x54, 0xff, 0x54}},
	{"b", {0x54, 0xff, 0xff}},
	{"c", {0xff, 0x54, 0x54}},
	{"d", {0xff, 0x54, 0xff}},
	{"e", {0xff, 0xff, 0x54}},
	{"f", {0xff,0xff,0xff}},
	{"g", {0xef, 0xce, 0x16}},
	{"h", {0xe2, 0xd3, 0xd1}},
	{"i", {0xce, 0xc9, 0xc9}},
	{"j", {0x44, 0x39, 0x3a}},
	{"m", {0x96, 0x15, 0x06}},
	{"n", {0xb4, 0x68, 0x4d}},
	{"p", {0xde, 0xb0, 0x2c}},
	{"q", {0x11, 0x9f, 0x36}},
	{"s", {0x2c, 0xb9, 0xa8}},
	{"t", {0x20, 0x48, 0x7a}},
	{"u", {0x9a, 0x5c, 0xc5}},
	{"t", {0x20, 0x48, 0x7a}}
};

class ImRenderUtil
{
public:
	inline static void drawText(Vector2<float> pos, std::string* textStr, const UIColor& color, float textSize, float alpha, bool shadow = false, int index = 0, ImDrawList* d = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;
		ImFont* font = ImGui::GetFont();



		ImVec2 textPos = ImVec2(pos.x, pos.y);
		constexpr ImVec2 shadowOffset = ImVec2(1.f, 1.f);
		constexpr ImVec2 shadowOffsetMinecraft = ImVec2(1.45f, 1.45f);

		if (shadow)
		{
			if (font == ImGui::GetIO().Fonts->Fonts[4]) {
				d->AddText(font, (textSize * 18), textPos + shadowOffsetMinecraft, ImColor(color.r * 0.2f, color.g * 0.2f, color.b * 0.2f, alpha * 0.7f), textStr->c_str());
			}
			else {
				d->AddText(font, (textSize * 18), textPos + shadowOffset, ImColor(color.r * 0.03f, color.g * 0.03f, color.b * 0.03f, alpha * 0.9f), textStr->c_str());
			}
		}

		d->AddText(font, (textSize * 18), textPos, ImColor(color.r, color.g, color.b, alpha), textStr->c_str());
	};

	static void textMCColor(Vector2<float> pos, std::string text, float fontSize = 10.f, ImFont* font = ImGui::GetFont()) {
		ImColor color = colors["f"];
		ImVec2 textPos = ImVec2(pos.x, pos.y);
		bool section = false;
		int posX = textPos.x;
		for (size_t i = 0; i < text.length();)
		{
			int cplen = 1;
			if ((text[i] & 0xf8) == 0xf0) cplen = 4;
			else if ((text[i] & 0xf0) == 0xe0) cplen = 3;
			else if ((text[i] & 0xe0) == 0xc0) cplen = 2;
			if ((i + cplen) > text.length()) cplen = 1;
			std::string c = text.substr(i, cplen);
			i += cplen;
			if (section) {
				section = false;
				if (colors.find(c) != colors.end()) {
					color = colors[c];
				}
				continue;
			}
			if (c == "\n") {
				color = colors["f"];
				textPos.x = posX;
				textPos.y += font->CalcTextSizeA(fontSize, FLT_MAX, -1, "").y;
			}
			else if (c == "\uFF82\uFF67") {
				section = true;
				continue;
			}
			auto textWidth = font->CalcTextSizeA(fontSize, FLT_MAX, -1, c.c_str()).x;
			ImGui::GetBackgroundDrawList()->AddText(font, fontSize, textPos, color, c.c_str());
			textPos.x += textWidth;
		}
	}

	static void drawGradientText(Vector2<float> pos, std::string* text, float size, float alpha, bool shadow, int speed, int index)
	{
		TextHolder str(*text);
		int ind = 0;

		for (char c : std::string(str.getText()))
		{
			std::string string = Utils::combine(c, "");

			// Current color index
			int colorIndex = ind * index;

			// Get each char's width and draw
			float charWidth = getTextWidth(&string, size);

			drawText(Vector2<float>(pos.x, pos.y), &string, ColorUtils::Rainbow(speed, 1.F, 1.F, colorIndex), size, alpha, shadow);

			// Increment the color and pos index
			pos.x += charWidth;
			++ind;
		}
	}

	template <typename T>
	static void fillRectangle(Vector4<T> pos, const UIColor& color, float alpha, float radius = 0.f, ImDrawList* list = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;

		list->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), radius);
	}

	template <typename T>
	static void Blur(Vector4<T> pos, float strenght, float radius = 0.f)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImFX::Begin(ImGui::GetBackgroundDrawList());
		ImFX::AddBlur(strenght, ImVec4(pos.x, pos.y, pos.z, pos.w), radius);
		ImFX::End();
	}

	template <typename T>
	static void fillRectangleCustom(Vector4<T> pos, const UIColor& color, float alpha, Vector4<float>radius) {
		if (!ImGui::GetCurrentContext()) return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		list->AddRectFilledCustomRadius(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), radius.x, radius.z, radius.y, radius.w);
	}

	static void fillShadowRectangle(Vector4<float> pos, const UIColor& color, float alpha, float thickness, ImDrawFlags flags, float radius = 0.f, ImDrawList* list = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImVec2 offset = ImVec2(0, 0);
		list->AddShadowRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), thickness, offset, flags, radius);
	}

	static void fillShadowCircle(Vector2<float> pos, float radius, const UIColor& color, float alpha, float thickness, ImDrawFlags flags, float segments = 12.f)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		ImVec2 offset = ImVec2(0, 0);
		list->AddShadowCircle(ImVec2(pos.x, pos.y), radius, ImColor(color.r, color.g, color.b, alpha), thickness, offset, flags, radius);
	}

	static void drawShadowSquare(Vector2<float> center, float size, const UIColor& color, float alpha, float thickness, ImDrawFlags flags)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		ImVec2 offset = ImVec2(0, 0);

		// Define the four corners of the square
		ImVec2 points[4];
		points[0] = ImVec2(center.x - size / 2.f, center.y - size / 2.f);
		points[1] = ImVec2(center.x + size / 2.f, center.y - size / 2.f);
		points[2] = ImVec2(center.x + size / 2.f, center.y + size / 2.f);
		points[3] = ImVec2(center.x - size / 2.f, center.y + size / 2.f);

		list->AddShadowConvexPoly(points, 4, ImColor(color.r, color.g, color.b, alpha), thickness, offset, flags);
	}

	static void drawRoundRect(Vector4<float> pos, const ImDrawFlags& flags, float radius, const UIColor& color, float alpha, float lineWidth)
	{
		if (!ImGui::GetCurrentContext())
			return;
		const auto d = ImGui::GetBackgroundDrawList();
		d->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), radius, flags, lineWidth);
	}

	static void fillGradientOpaqueRectangle(Vector4<float> pos, const UIColor& firstColor, const UIColor& secondColor, float firstAlpha, float secondAlpha)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		list->AddRectFilledMultiColor(topLeft, bottomRight,
			ImColor(firstColor.r, firstColor.g, firstColor.b, secondAlpha),
			ImColor(secondColor.r, secondColor.g, secondColor.b, secondAlpha),
			ImColor(secondColor.r, secondColor.g, secondColor.b, firstAlpha),
			ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha));
	}

	static void fillRoundedGradientRectangle(Vector4<float> pos, const UIColor& firstColor, const UIColor& secondColor, float radius, float firstAlpha, float secondAlpha)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		list->AddRectFilledMultiColor(topLeft, bottomRight,
			ImColor(firstColor.r, firstColor.g, firstColor.b, secondAlpha),
			ImColor(secondColor.r, secondColor.g, secondColor.b, secondAlpha),
			ImColor(secondColor.r, secondColor.g, secondColor.b, firstAlpha),
			ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha));
	}

	static void fillCircle(Vector2<float> center, float radius, const UIColor& color, float alpha, int segments)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		list->AddCircleFilled(ImVec2(center.x , center.y), radius, ImColor(color.r, color.g, color.b, alpha), segments);
	}

	static inline Vector2<float> getScreenSize() {
		RECT desktop;

		GetWindowRect(window, &desktop);
		int w = desktop.right - desktop.left;
		int h = desktop.bottom - desktop.top;
		// not in fullscreen
		if (desktop.top != 0 || desktop.left != 0) {
			//w -= 30;
			//h -= 45;
		}
		return Vector2<float>(w, h);
	}

	static bool isFullScreen() {
		RECT desktop;

		GetWindowRect(window, &desktop);
		if (desktop.top != 0 || desktop.left != 0)
			return false;
		return true;
	}

	static inline float getTextHeightStr(std::string* textStr, float textSize) {
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).y;
	}

	static inline float getTextWidth(std::string* textStr, float textSize)
	{
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).x;
	}

	static inline float getTextHeight(float textSize)
	{
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, "").y;
	}

	static inline float getDeltaTime() {
		return 0.016f;
	}

	static inline Vector2<float> getMousePos() {
		if (!Global::getClientInstance()->getGuiData()) {
			return { 0, 0 };
		}
		return Global::getClientInstance()->getGuiData()->getMousePos().toFloat();
	}

	inline static bool isMouseOver(Vector4<float>(pos))
	{
		Vector2<short> mousePos = Global::getClientInstance()->getGuiData()->getMousePos();
		return mousePos.x >= pos.x && mousePos.y >= pos.y && mousePos.x < pos.z && mousePos.y < pos.w;
	}

	static __forceinline void drawLine(Vector2<float> start, Vector2<float> end, UIColor color, float lineWidth) {
		if (!ImGui::GetCurrentContext()) return;
		const auto d = ImGui::GetBackgroundDrawList();
		d->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), ImColor(color.r, color.g, color.b, color.a), lineWidth);
	}

	static Vector2<float> TestWorldToScreen(const Vector3<float>& world) {  //
		if (ImGui::GetCurrentContext()) {
			Vector2<float> ret;
			Vector2<float> windowSize = Vector2<float>(getScreenSize().x + (isFullScreen() ? 0 : 10), getScreenSize().y - (isFullScreen() ? -10 : 5));
			Global::getClientInstance()->WorldToScreen(world, ret);
			return ret;
		}
	}

	static void drawLine3D(const Vector3<float>& start, const Vector3<float>& end, UIColor color) {
		if (!ImGui::GetCurrentContext()) return;

		Vector2<float> startScreen = TestWorldToScreen({ start.x, start.y, start.z });
		Vector2<float> endScreen = TestWorldToScreen({ end.x, end.y, end.z });

		drawLine(startScreen, endScreen, color, 1.4);
	}

	static void drawRing3D(const Vector3<float>& center, float radius, int segments, float speed) {
		if (!ImGui::GetCurrentContext()) return;
		std::vector<Vector3<float>> points;
		for (int i = 0; i < segments; ++i) {
			float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * PI;
			float x = center.x + radius * std::cos(angle);
			float y = center.y;
			float z = center.z + radius * std::sin(angle);
			points.push_back(Vector3<float>(x, y, z));
		}

		int ind = 0;

		for (size_t i = 0; i < points.size() - 1; ++i) {
			//Vector2<float> Pos1;
			int colorIndex = ind * 7;
			Vector2 Pos1 = TestWorldToScreen(center);
			if (!Global::getClientInstance()->WorldToScreen(center, Pos1)) continue;
			drawLine3D(points[i], points[i + 1], UIColor(255,255,255));
			++ind;
		}
		drawLine3D(points.back(), points.front(), ColorUtils::Rainbow(speed, 1.F, 1.F, ind * 7));
	}

	static __forceinline void drawBox(Vector3<float> lower, Vector3<float> upper, UIColor color, UIColor lineColor, float lineWidth, bool fill, bool outline) {
		Vector3<float> diff;
		diff.x = upper.x - lower.x;
		diff.y = upper.y - lower.y;
		diff.z = upper.z - lower.z;

		//Vector3<float> diff = upper.submissive(lower);
		Vector3<float> vertices[8];
		vertices[0] = Vector3<float>(lower.x, lower.y, lower.z);
		vertices[1] = Vector3<float>(lower.x + diff.x, lower.y, lower.z);
		vertices[2] = Vector3<float>(lower.x, lower.y + diff.y, lower.z);
		vertices[3] = Vector3<float>(lower.x + diff.x, lower.y + diff.y, lower.z);
		vertices[4] = Vector3<float>(lower.x, lower.y, lower.z + diff.z);
		vertices[5] = Vector3<float>(lower.x + diff.x, lower.y, lower.z + diff.z);
		vertices[6] = Vector3<float>(lower.x, lower.y + diff.y, lower.z + diff.z);
		vertices[7] = Vector3<float>(lower.x + diff.x, lower.y + diff.y, lower.z + diff.z);

		auto instance = Global::getClientInstance();

		const auto d = ImGui::GetBackgroundDrawList();

		if (fill) {
			// Convert the vertices to screen coordinates
			std::vector<Vector2<float>> screenCords;
			for (int i = 0; i < 8; i++) {
				Vector2<float> screen;
				if (instance->WorldToScreen(vertices[i], screen)) {
					screenCords.push_back(screen);
				}
			}

			// Return if there are less than four points to draw quads with
			if (screenCords.size() < 8) return;

			// Define the indices of the vertices to use for each quad face
			std::vector<std::tuple<int, int, int, int>> faces = {
				{0, 1, 3, 2},  // Bottom face
				{4, 5, 7, 6},  // Top face
				{0, 1, 5, 4},  // Front face
				{2, 3, 7, 6},  // Back face
				{1, 3, 7, 5},  // Right face
				{0, 2, 6, 4}   // Left face
			};

			// Draw the quads to fill the box
			for (auto face : faces) {
				ImVec2 posScreenCords = ImVec2(screenCords[std::get<0>(face)].x, screenCords[std::get<0>(face)].y);
				ImVec2 posScreenCords1 = ImVec2(screenCords[std::get<1>(face)].x, screenCords[std::get<1>(face)].y);
				ImVec2 posScreenCords2 = ImVec2(screenCords[std::get<2>(face)].x, screenCords[std::get<2>(face)].y);
				ImVec2 posScreenCords3 = ImVec2(screenCords[std::get<3>(face)].x, screenCords[std::get<3>(face)].y);
				d->AddQuadFilled(posScreenCords, posScreenCords1, posScreenCords2, posScreenCords3, ImColor(color.r, color.g, color.b, color.a));
				//drawlist->AddQuadFilled(screenCords[std::get<3>(face)].toImVec2(), screenCords[std::get<2>(face)].toImVec2(), screenCords[std::get<1>(face)].toImVec2(), screenCords[std::get<0>(face)].toImVec2(), color.toImColor());
			}
		}

		{
			// Convert the vertices to screen coordinates
			std::vector<std::tuple<int, Vector2<float>>> screenCords;
			for (int i = 0; i < 8; i++) {
				Vector2<float> screen;
				if (instance->WorldToScreen(vertices[i], screen)) {
					screenCords.emplace_back(outline ? (int)screenCords.size() : i, screen);
				}
			}

			// Return if there are less than two points to draw lines between
			if (screenCords.size() < 2) return;

			switch (outline) {
			case false: {
				// Draw lines between all pairs of vertices
				for (auto it = screenCords.begin(); it != screenCords.end(); it++) {
					auto from = *it;
					auto fromOrig = vertices[std::get<0>(from)];

					for (auto to : screenCords) {
						auto toOrig = vertices[std::get<0>(to)];

						// Determine if the line should be drawn based on the relative positions of the vertices
						bool shouldDraw = false;
						// X direction
						shouldDraw |= fromOrig.y == toOrig.y && fromOrig.z == toOrig.z && fromOrig.x < toOrig.x;
						// Y direction
						shouldDraw |= fromOrig.x == toOrig.x && fromOrig.z == toOrig.z && fromOrig.y < toOrig.y;
						// Z direction
						shouldDraw |= fromOrig.x == toOrig.x && fromOrig.y == toOrig.y && fromOrig.z < toOrig.z;

						ImVec2 posForm = ImVec2(std::get<1>(from).x, std::get<1>(from).y);
						ImVec2 posTo = ImVec2(std::get<1>(to).x, std::get<1>(to).y);
						if (shouldDraw) d->AddLine(posForm, posTo, ImColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a), lineWidth);
					}
				}
				return;
				break;
			}
			case true: {
				// Find start vertex
				auto it = screenCords.begin();
				std::tuple<int, Vector2<float>> start = *it;
				it++;
				for (; it != screenCords.end(); it++) {
					auto cur = *it;
					if (std::get<1>(cur).x < std::get<1>(start).x) {
						start = cur;
					}
				}

				// Follow outer line
				std::vector<int> indices;

				auto current = start;
				indices.push_back(std::get<0>(current));
				Vector2<float> lastDir(0, -1);
				do {
					float smallestAngle = PI * 2;
					Vector2<float> smallestDir;
					std::tuple<int, Vector2<float>> smallestE;
					auto lastDirAtan2 = atan2(lastDir.y, lastDir.x);
					for (auto cur : screenCords) {
						if (std::get<0>(current) == std::get<0>(cur))
							continue;

						// angle between vecs
						Vector2<float> dir = Vector2<float>(std::get<1>(cur)).submissive(std::get<1>(current));
						float angle = atan2(dir.y, dir.x) - lastDirAtan2;
						if (angle > PI) {
							angle -= 2 * PI;
						}
						else if (angle <= -PI) {
							angle += 2 * PI;
						}
						if (angle >= 0 && angle < smallestAngle) {
							smallestAngle = angle;
							smallestDir = dir;
							smallestE = cur;
						}
					}
					indices.push_back(std::get<0>(smallestE));
					lastDir = smallestDir;
					current = smallestE;
				} while (std::get<0>(current) != std::get<0>(start) && indices.size() < 8);

				// draw

				Vector2<float> lastVertex;
				bool hasLastVertex = false;
				for (auto& indice : indices) {
					Vector2<float> curVertex = std::get<1>(screenCords[indice]);
					if (!hasLastVertex) {
						hasLastVertex = true;
						lastVertex = curVertex;
						continue;
					}
					ImVec2 lastVertexPos = ImVec2(lastVertex.x, lastVertex.y);
					ImVec2 curVertexPos = ImVec2(curVertex.x, curVertex.y);
					d->AddLine(lastVertexPos, curVertexPos, ImColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a), lineWidth);
					lastVertex = curVertex;
				}
				return;
				break;
			}
			}
		}
	}

	static __forceinline void drawCorners(Vector3<float> lower, Vector3<float> upper, UIColor color, float lineWidth) {
		if (Global::getClientInstance()->getLocalPlayer() == nullptr) return;
		Vector3<float> worldPoints[8];
		worldPoints[0] = Vector3<float>(lower.x, lower.y, lower.z);
		worldPoints[1] = Vector3<float>(lower.x, lower.y, upper.z);
		worldPoints[2] = Vector3<float>(upper.x, lower.y, lower.z);
		worldPoints[3] = Vector3<float>(upper.x, lower.y, upper.z);
		worldPoints[4] = Vector3<float>(lower.x, upper.y, lower.z);
		worldPoints[5] = Vector3<float>(lower.x, upper.y, upper.z);
		worldPoints[6] = Vector3<float>(upper.x, upper.y, lower.z);
		worldPoints[7] = Vector3<float>(upper.x, upper.y, upper.z);

		std::vector<Vector2<float>> points;
		for (int i = 0; i < 8; i++) {
			Vector2<float> result;
			if (Global::getClientInstance()->WorldToScreen(worldPoints[i], result))
				points.emplace_back(result);
		}
		if (points.size() < 1) return;

		Vector4<float> resultRect = { points[0].x, points[0].y, points[0].x, points[0].y };
		for (const auto& point : points) {
			if (point.x < resultRect.x) resultRect.x = point.x;
			if (point.y < resultRect.y) resultRect.y = point.y;
			if (point.x > resultRect.z) resultRect.z = point.x;
			if (point.y > resultRect.w) resultRect.w = point.y;
		}

		float length = (resultRect.x - resultRect.z) / 4.f;

		// Top left
		drawLine(Vector2(resultRect.x, resultRect.y), Vector2(resultRect.x - length, resultRect.y), color, lineWidth);
		drawLine(Vector2(resultRect.x, resultRect.y), Vector2(resultRect.x, resultRect.y - length), color, lineWidth);

		// Top right
		drawLine(Vector2(resultRect.z, resultRect.y), Vector2(resultRect.z + length, resultRect.y), color, lineWidth);
		drawLine(Vector2(resultRect.z, resultRect.y), Vector2(resultRect.z, resultRect.y - length), color, lineWidth);

		// Bottom left
		drawLine(Vector2(resultRect.x, resultRect.w), Vector2(resultRect.x - length, resultRect.w), color, lineWidth);
		drawLine(Vector2(resultRect.x, resultRect.w), Vector2(resultRect.x, resultRect.w + length), color, lineWidth);

		// Bottom right
		drawLine(Vector2(resultRect.z, resultRect.w), Vector2(resultRect.z + length, resultRect.w), color, lineWidth);
		drawLine(Vector2(resultRect.z, resultRect.w), Vector2(resultRect.z, resultRect.w + length), color, lineWidth);
	}
};

class ImRotateUtil {
public:
	ImRotateUtil() {
		rotationStartIndex = 0.f;
	}

	static void startRotation() {
		rotationStartIndex = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
	}

	static ImVec2 getRotationCenter() {
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

		const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
		for (int i = rotationStartIndex; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
	}

	static void endRotation(float rad, ImVec2 center = getRotationCenter()) {
		rad += PI * 0.5f;
		float s = sin(rad), c = cos(rad);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;

		for (int i = rotationStartIndex; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}

private:
	inline static int rotationStartIndex = 0;
};

class ParticleEngine
{
public:
	struct Particle {
		float x, y, vx, vy, lifetime, opacity, size, distance, delta;
		UIColor color;

		Particle() : x(0), y(0), vx(0), vy(0), lifetime(0), opacity(0), color(UIColor(0, 0, 0)), size(0), distance(0), delta(0) {}
		Particle(float x, float y, float vx, float vy, float lifetime, float opacity, UIColor color, float size, float distance, float delta)
			: x(x), y(y), vx(vx), vy(vy), lifetime(lifetime), opacity(opacity), color(color), size(size), distance(distance), delta(delta) {}
	};

	std::vector<Particle> particles;

	void addParticles(int numParticles, float startX, float startY, float distance, float size, float seconds = 4.f)
	{
		for (int i = 0; i < numParticles; i++)
		{
			Particle p;
			p.x = startX + rand() % 10 - 5;
			p.y = startY + rand() % 10 - 5;
			p.vx = rand() % 200 - 100 + rand() % 10 - 5;
			p.vy = rand() % 200 - 100 + rand() % 10 - 5;
			p.lifetime = seconds;
			p.opacity = 1.0f;
			p.color = ColorUtils::Rainbow(1, 1, 1, p.x * 45);
			p.size = size + rand() % ((int)size / 2 + 1);
			p.distance = distance;
			particles.push_back(p);
		}
	}

	void clearParticles() {
		particles.clear();
	}

	void updateParticles(float delta) {
		for (auto& p : particles) {
			EasingUtil easing;
			easing.percentage = p.lifetime / p.distance;
			float easedPercentage = easing.easeOutElastic();

			float targetX = Math::lerp(p.x, p.x + p.vx * delta, easedPercentage);
			float targetY = Math::lerp(p.y, p.y + p.vy * delta, easedPercentage);

			p.x = Math::lerp(p.x, targetX, easedPercentage);
			p.y = Math::lerp(p.y, targetY, easedPercentage);

			p.lifetime -= delta;
			p.opacity -= delta * 0.3f;
		}

		particles.erase(std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.lifetime <= 0; }), particles.end());

		if (particles.size() > 250) {
			particles.resize(250);
		}
	}

	void drawParticles() {
		for (const auto& p : particles) {
			Vector2<float> center(p.x + p.size * 0.5f, p.y + p.size * 0.5f);
			float radius = p.size * 0.5f;
			int segments = 100;

			ImRenderUtil::fillCircle(center, radius, p.color, p.opacity, segments);
		}
	}
};