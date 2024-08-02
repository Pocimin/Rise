#pragma once

class BlockLegacy
{
public:
	TextHolder* GetTileName()
	{
		return reinterpret_cast<TextHolder*>((uintptr_t)this + 0x8); // 1.20.80
	}

	int getBlockID()
	{
		return *reinterpret_cast<int*>((uintptr_t)this + 0x1A6); // 1.20.80
	}

	bool isSolid() {
		return *reinterpret_cast<bool*>((uintptr_t)this + 0x16); // 1.20.80
	}

	Material* GetMaterial()
	{
		return *reinterpret_cast<Material**>((uintptr_t)this + 0x128); // 1.20.80
	}

	MaterialType GetMaterialType()
	{
		return GetMaterial()->type;
	}
};