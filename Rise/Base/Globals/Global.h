#pragma once

// You might use this if you want to group all of your global variables,
// functions, etc. under a common name to make them easier to find and organize
namespace Global {
	static inline std::map<uint64_t, bool> Keymap = {}; // Keymap for key checks.

	// Rotations
	static inline float headYaw = 0.f;
	static inline float bodyYaw = 0.f;
	static inline float pitch = 0.f;

	static inline float easedHeadYaw = 0.f;
	static inline float easedBodyYaw = 0.f;
	static inline float easedPitch = 0.f;
	
	static inline float HeadYawSpeed = 2.07;
	static inline float BodyYawSpeed = 1.04;
	static inline float PitchSpeed = 2.07;

	static inline bool lockBodyYaw = true;

	// Interface
	struct HoverTextInfo {
		bool show = false;
		std::string text = "";
		Vector2<float> pos{ 0,0 };
	};

	static inline bool doChams = true;

	HoverTextInfo info;

	// Notifications
	static inline NotificationsManager Notification{};
	static inline bool RenderNotifications = true;

	// User and Auth
	static inline std::string Username = "Unknown";
	static inline bool renderUI = false;

	// Speed
	static inline Vector3<float> lastLerpVelocity = NULL;
	static inline bool effectSwiftness = false;

	// Regen
	static inline bool isMiningRedstone = false;
	static inline bool shouldAttack = true;
	static inline Vector3<int> miningPosition = NULL;

	static inline bool isOnFire = false;

	// Colours
	static inline int ArrayListColors = 0;

	// Scaffold
	static inline int SwingType = false;

	// IsAtacking
	static inline bool isAttacking = false;

	// LayerName
	static inline std::string LayerName = "FuckMeUnknown";

	static inline float AmbienceTime = 15000;
	static inline float BreakSpeed = 10;

	// Desync position
	static inline Vector3<float> testPosition(0, 0, 0);

	// ShouldBlock
	static inline bool ShouldBlock = false;
	static inline float swingSpeed = 13.f;

	// Killsults
	static inline bool Killed = false; // Weither you killed a player or not
	static inline std::string KilledTarget = "here"; // The target player name
	static inline int KilledTicks = 0;

	// NetSkip
	static inline bool shouldLagTicks = false;

	// In Class functions, variables, etc. (to avoid "is not a type name" error)
	class Core {
	public:
		static inline ClientInstance* ClientInstance = nullptr; // ClientInstance
		//static inline Player* Player = nullptr;
		static inline LoopbackPacketSender* LoopbackPacketSender = nullptr;
		static inline Packet* Packet = nullptr;

		static inline bool ShouldAnimate = false;
	};

	// Change or Set ClientInstance
	static void setClientInstance(ClientInstance* instance) { Core::ClientInstance = instance; }

	// Get ClientInstance's class
	ClientInstance* getClientInstance() { return Core::ClientInstance; }
	// Get Player's class
	//Player* getLocalPlayer() { return getClientInstance()->getLocalPlayer(); }
	// Get LoopbackPacketSender's class
	LoopbackPacketSender* getLoopbackPacketSender() { return Core::LoopbackPacketSender; }
	// Get Packet's class
	Packet* getPacket() { return Core::Packet; }

	namespace RenderInfo {
		static inline HWND Window = nullptr;
		static inline ID3D11ShaderResourceView* HeadTexture = NULL;
		static inline ID3D11ShaderResourceView* CombatTexture = NULL;
		static inline ID3D11ShaderResourceView* MovementTexture = NULL;
		static inline ID3D11ShaderResourceView* PlayerTexture = NULL;
		static inline ID3D11ShaderResourceView* VisualTexture = NULL;
		static inline ID3D11ShaderResourceView* MiscTexture = NULL;
		static inline ID3D11ShaderResourceView* UserProfileTexture = NULL;
	}
}
