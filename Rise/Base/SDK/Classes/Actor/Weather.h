#pragma once

class Weather {
public:
	BUILD_ACCESS(float, fogLevel, 0x4C); // 1.20.80
	BUILD_ACCESS(float, lightingLevel, 0x44); // 1.20.80
	BUILD_ACCESS(float, rainLevel, 0x38); // 1.20.80
};