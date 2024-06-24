#pragma once

class MobEquipmentPacket : public Packet
{
public:
	__int64    mRuntimeId;        // this+0x30
	void*      mItem;             // this+0x38
	int        mSlot;             // this+0x98
	int        mSelectedSlot;     // this+0x9C
	uint8_t    mContainerId;      // this+0xA0
	uint8_t    mSlotByte;         // this+0xA1
	uint8_t    mSelectedSlotByte; // this+0xA2
	uint8_t    mContainerIdByte;  // this+0xA3

	// ActorRuntimeID
	// NetworkItemStackDescriptor
	// int invSlot
	// int hotbarSlot
	// ContainerID
	// uchar invSlot
	// uchar hotbarSlot
	// uchar
}; //Size: 0x0170