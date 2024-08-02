#pragma once

enum class InputData : int {
    Ascend = 0x0,
    Descend = 0x1,
    NorthJump = 0x2,
    JumpDown = 0x3,
    SprintDown = 0x4,
    ChangeHeight = 0x5,
    Jumping = 0x6,
    AutoJumpingInWater = 0x7,
    Sneaking = 0x8,
    SneakDown = 0x9,
    Up = 0xA,
    Down = 0xB,
    Left = 0xC,
    Right = 0xD,
    UpLeft = 0xE,
    UpRight = 0xF,
    WantUp = 0x10,
    WantDown = 0x11,
    WantDownSlow = 0x12,
    WantUpSlow = 0x13,
    Sprinting = 0x14,
    AscendBlock = 0x15,
    DescendBlock = 0x16,
    SneakToggleDown = 0x17,
    PersistSneak = 0x18,
    StartSprinting = 0x19,
    StopSprinting = 0x1A,
    StartSneaking = 0x1B,
    StopSneaking = 0x1C,
    StartSwimming = 0x1D,
    StopSwimming = 0x1E,
    StartJumping = 0x1F,
    StartGliding = 0x20,
    StopGliding = 0x21,
    PerformItemInteraction = 0x22,
    PerformBlockActions = 0x23,
    PerformItemStackRequest = 0x24,
    HandledTeleport = 0x25,
    Emoting = 0x26,
    MissedSwing = 0x27,
    StartCrawling = 0x28,
    StopCrawling = 0x29,
    StartFlying = 0x2A,
    StopFlying = 0x2B,
    ReceivedServerData = 0x2C,
    InClientPredictedInVehicle = 0x2D,
    PaddlingLeft = 0x2E,
    PaddlingRight = 0x2F,
};

enum class InputMode : int {
    Undefined = 0x0,
    Mouse = 0x1,
    Touch = 0x2,
    GamePad = 0x3,
    MotionController = 0x4,
};

enum class ClientPlayMode : int {
    Normal = 0x0,
    Teaser = 0x1,
    Screen = 0x2,
    Viewer = 0x3,
    Reality = 0x4,
    Placement = 0x5,
    LivingRoom = 0x6,
    ExitLevel = 0x7,
    ExitLevelLivingRoom = 0x8,
    NumModes = 0x9,
};

class PlayerAuthInputPacket : public Packet {
public:
	/* Fields */
	/*Vector2<float> rotation; //0x0030
	Vector3<float> position; //0x0038
	float headYaw; //0x0044
	char pad_0048[4]; //0x0048
	float downVelocity; //0x004C
	char pad_0050[32]; //0x0050
	void* inputData; //0x0070
	void* inputMode; //0x0078
	void* playMode; //0x007C
	char pad_0080[8]; //0x0080
	int32_t ticksAlive; //0x0088
	char pad_008C[44]; //0x008C
    */
	Vector2<float>                                           rotation;             // this+0x30
	Vector3<float>                                           position;             // this+0x38
	float                                                    headYaw;              // this+0x44
	Vector3<float>                                           mPosDelta;            // this+0x4C
	Vector2<float>                                           mAnalogMoveVector;    
	Vector2<float>                                           mVehicleRotation;
	Vector2<float>                                           mMove;
	Vector3<float>                                           mGazeDir;
	InputData                                                mInputData;           // this+0x70
	InputMode                                                mInputMode;           // this+0x78
	ClientPlayMode                                           mPlayMode;            // this+0x7C
	//NewInteractionModel                                      mNewInteractionModel; //
	int32_t                                                  TicksAlive;           // this+0x88
	/*std::unique_ptr<PackedItemUseLegacyInventoryTransaction> mItemUseTransaction;
	std::unique_ptr<ItemStackRequestData>                    mItemStackRequest;
	PlayerBlockActions                                       mPlayerBlockActions;
	ActorUniqueID                                            mPredictedVehicle;*/

	/* Virtuals */
	/* Functions */
	PlayerAuthInputPacket() {};
	PlayerAuthInputPacket(const PlayerAuthInputPacket& other) : Packet()
	{
		memcpy(this, &other, sizeof(PlayerAuthInputPacket));
	}
	void operator=(const PlayerAuthInputPacket& other)
	{
		memcpy(this, &other, sizeof(PlayerAuthInputPacket));
	}
};