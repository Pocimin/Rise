#pragma once

// Declare a void pointer(Empty object) called onRakpeer which is the CallBack for RakpeerDetour.
void* onRakpeer; // Defined an orignal for Rakpeer.

__int64 RakpeerDetour(RakPeer* _this, __int64 a1) {
	if (Global::shouldLagTicks && Global::getClientInstance()->getMinecraftGame()->getCanUseKeys()) {
		return 0;
	}

	//FileUtils::debugOutput(Utils::combine("Ping: ", _this->getPing()));

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	return Utils::CallFunc<__int64, RakPeer*, __int64>( // CallFunc to call the original.
		onRakpeer, _this, a1
	);
}

class RakpeerHook : public FuncHook { // a Rakpeer class that inherits the FuncHook template we made
public:
	bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
	{
		// Using the utils class findSig function to locate the address of the signature
		// 
		// RakPeer_RunUpdateCycle
		// 48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B FA 48 89 55 A0 4C 8B F1 48 89 4D A8 33 FF 89 7C 24 70 48 8D 8D ? ? ? ? E8 ? ? ? ? 45 33 ED 4C 89 6C 24 ? 33 C0 48 89 44 24 ? 49 8B 8E ? ? ? ?
		// 48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B FA 48 89 54 24 ? 48 8B F1 48 89 4C 24 ? 48 89 4C 24 ? 45 33 E4 44 89 64 24 ? E8 ? ? ? ? C7 44 24 ? ? ? ? ? 49 8D 97 ? ? ? ? 48 8B CE E8

		auto VTable = *(uintptr_t**)Global::getClientInstance()->getLoopbackPacketSender()->getNetworkSystem()->getRemoteConnectorComposite()->getRakNetConnector()->getPeer();

		// Attempt to hook the function at the address stored in LoopbackVTable by replacing it with the address of RakpeerDetour and store the original function's address in onRakpeer
        return Utils::HookFunction((void*)VTable[91], (void*)&RakpeerDetour, &onRakpeer, xorstr_("RakPeer"));

		return true;
	}

	static RakpeerHook& Instance() { // a class setup function called Instance.
		static RakpeerHook instance;
		return instance;
	}
};