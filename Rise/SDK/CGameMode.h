#pragma once

#include "CClientInstance.h"
#include "CEntity.h"

#include <stdint.h>
#include "../Memory/MinHook.h"
#include "../Utils/HMath.h"

class C_GameMode {
private:
	virtual __int64 destructorGameMode();
	// Duplicate destructor
public:
	virtual __int64 startDestroyBlock(Vector3i const&, unsigned char, bool &);
	virtual __int64 destroyBlock(Vector3i*, unsigned char);
	virtual __int64 continueDestroyBlock(Vector3i const&, unsigned char, bool &);
	virtual __int64 stopDestroyBlock(Vector3i const&);
	virtual __int64 startBuildBlock(Vector3i const&, unsigned char);
	virtual __int64 buildBlock(Vector3i *, unsigned char);
	virtual __int64 continueBuildBlock(Vector3i const&, unsigned char);
	virtual __int64 stopBuildBlock(void);
	virtual __int64 tick(void);
private:
	virtual __int64 getPickRange(__int64 const&, bool);
	virtual __int64 useItem(__int64 &);
	virtual __int64 useItemOn(__int64 &, Vector3i const&, unsigned char, Vector3 const&, __int64 const*);
	virtual __int64 interact(C_Entity &, Vector3 const&);
public:
	virtual __int64 attack(C_Entity*);
private:
	virtual __int64 releaseUsingItem(void);
public:
	virtual void setTrialMode(bool);
	virtual bool isInTrialMode(void);
private:
	virtual __int64 registerUpsellScreenCallback(__int64);
public:
	C_Entity* player;
};