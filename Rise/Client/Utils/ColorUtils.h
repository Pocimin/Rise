#pragma once

class ColorUtils
{
public:
	inline static UIColor Rainbow(float seconds, float saturation, float brightness, int index) {
		float hue = ((TimeUtils::getCurrentMs() + index) % (int)(seconds * 1000)) / (float)(seconds * 1000);
		float r, g, b = 0;
		hsvToRgb(hue, saturation, brightness, r, g, b);
		return UIColor(r * 255, g * 255, b * 255);
	}

	inline static UIColor RainbowDark(float speed, int index) {
		float hue = ((TimeUtils::getCurrentMs() + index) % (int)(speed * 1000)) / (float)(speed * 1000);
		float r, g, b = 0;
		hsvToRgb(hue, 1.F, 1.F, r, g, b);
		return UIColor(r * 180, g * 180, b * 190);
	}

	inline static UIColor GradientColor(float speed, int red, int green, int blue, int red2, int green2, int blue2, long index) {
		double offset = ((TimeUtils::getCurrentMs() - index) / 8) / (double)120;

		double primaryHue = ((TimeUtils::getCurrentMs() - index) % 1000 / 1000.000);

		int secoundaryHue = ((TimeUtils::getCurrentMs() - index) % 2000 / 2000.000) * 2;

		primaryHue = secoundaryHue % 2 == 0 ? primaryHue : 1 - primaryHue;
		double inverse_percent = 1 - primaryHue;
		int redPart = (int)(red * inverse_percent + red2 * primaryHue);
		int greenPart = (int)(green * inverse_percent + green2 * primaryHue);
		int bluePart = (int)(blue * inverse_percent + blue2 * primaryHue);

		return UIColor(redPart, greenPart, bluePart);
	}
private:
	inline static void hsvToRgb(float h, float s, float v, float& rOut, float& gOut, float& bOut)
	{

		if (s == 0.0f)
		{
			// gray
			rOut = gOut = bOut = v;
			return;
		}

		h = fmodf(h, 1.0f) / (60.0f / 360.0f);
		int i = (int)h;
		float f = h - (float)i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0:
			rOut = v;
			gOut = t;
			bOut = p;
			break;
		case 1:
			rOut = q;
			gOut = v;
			bOut = p;
			break;
		case 2:
			rOut = p;
			gOut = v;
			bOut = t;
			break;
		case 3:
			rOut = p;
			gOut = q;
			bOut = v;
			break;
		case 4:
			rOut = t;
			gOut = p;
			bOut = v;
			break;
		case 5:
		default:
			rOut = v;
			gOut = p;
			bOut = q;
			break;
		}
	}
};
