#pragma once

void* onLoopbackPacketSender;
void* onTextPacketDispatcher;
void* onDisconnectPacketDispatcher;

void LoopbackPacketSenderDetour(LoopbackPacketSender* _this, Packet* packet) {
    bool cancelled = false;
	PacketID id = (PacketID)packet->getId();

	if (packet->getId() == PacketID::PlayerAuthInput) {
		auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(packet);
		if (pkt) {
			Global::bodyYaw = pkt->rotation.y;
			Global::headYaw = pkt->headYaw;
			Global::pitch = pkt->rotation.x;

			Global::testPosition = pkt->position;
		}
	}

	if (strcmp(packet->getTypeName().getText(), "TextPacket") == 0) {
		std::string* message = (std::string*)(__int64)((char*)packet + 0x58);
		if (message->c_str()[0] == *"." || message->c_str()[0] == *",")
		{
			CommandMgr.sendCommand(message->c_str());
			return;
		}
	}

    PacketEvent event{ _this, packet }; // PacketEvent
    event.cancelled = &cancelled;
    CallBackEvent(&event); // Call Packet event for modules to be writen on this hook.

	Global::Core::LoopbackPacketSender = _this;
	Global::Core::Packet = packet;

    if (!cancelled) {
        // Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
        Utils::CallFunc<void*, LoopbackPacketSender*, Packet*>( // CallFunc to call the original.
            onLoopbackPacketSender, _this, packet
        );
    }
}

bool ContainsIgnoreCase(std::string str, std::string find) {
	std::string mainStrLower = str;
	std::string subStrLower = find;

	// Convert both mainStr and subStr to lowercase
	std::transform(mainStrLower.begin(), mainStrLower.end(), mainStrLower.begin(), ::tolower);
	std::transform(subStrLower.begin(), subStrLower.end(), subStrLower.begin(), ::tolower);

	// Perform the containment check
	return mainStrLower.find(subStrLower) != std::string::npos;
}

void TextPacketDispatcherDetour(const float* a1, const float* networkIdentifier, const float* netEventCallback, const std::shared_ptr<Packet>& packet) {
	PacketID id = packet.get()->getId();

	if (id == PacketID::Text) {
		auto* pkt = reinterpret_cast<TextPacket*>(packet.get());

		std::string message = pkt->message;

		message = Utils::sanitize(message);

		std::string playerNameTag = Global::getClientInstance()->getLocalPlayer()->getNametag()->c_str();

		if (playerNameTag.find("\n") != std::string::npos)
			playerNameTag = playerNameTag.substr(0, playerNameTag.find("\n"));

		playerNameTag = Utils::sanitize(playerNameTag);

		if (ContainsIgnoreCase(message, playerNameTag + " killed")) {
			std::string target = message.substr(message.find("killed") + 7, message.find(playerNameTag) - message.find("killed") - 8);
			target = Utils::sanitize(target);

			if (ContainsIgnoreCase(target, " ")) 
				target = "\"" + target + "\"";

			Global::KilledTarget = target;
			Global::Killed = true;
			ChatUtils::sendMessage("yeah some funny shit");
		}
	}

	Utils::CallFunc<void*, const float*, const float*, const float*, const std::shared_ptr<Packet>&>( // CallFunc to call the original.
		onTextPacketDispatcher, a1, networkIdentifier, netEventCallback, packet
	);
}

void DisconnectPacketDispatcherDetour(const float* a1, const float* networkIdentifier, const float* netEventCallback, const std::shared_ptr<Packet>& packet) {
	PacketID id = packet.get()->getId();

	if (id == PacketID::Disconnect) {
		if (getModuleByName("kickdecryptor")->isEnabled()) {
			auto player = Global::getClientInstance()->getLocalPlayer();

			if (!player) return;

			auto disconnectPacket = std::reinterpret_pointer_cast<DisconnectPacket>(packet);
			std::string reason = disconnectPacket->mMessage;

			// Make sure the reason starts with "Error: " (The rest of the msg is a base64 code)
		    // Remove the "Error: " part

			if (!ContainsIgnoreCase(reason, "Error: ")) return;

			reason = reason.substr(10);

			std::vector<uint8_t> bytes = base64_decode(reason);

			// Remove the first 24 bytes
			bytes.erase(bytes.begin(), bytes.begin() + 24);

			unsigned int size = bytes[0];
			unsigned int readIndex = 1;

			std::string flags = "";
			std::string coloredFlags = "";

			// Read the flags
			for (int i = 0; i < size; i++)
			{
				uint8_t flag = bytes[readIndex];
				uint8_t flagType = bytes[readIndex + 1];
				readIndex += 2;

				std::string flagName = GetFlagName(flag);
				std::string flagTypeStr = GetType(flagType);

				flags += flagName + " (Type: " + flagTypeStr + ")" + "\n";
				coloredFlags += "§a" + flagName + "§7 (Type: §c" + flagTypeStr + "§7)\n";
			}

			// Remove the last \n
			flags = flags.substr(0, flags.length() - 1);

			std::string playerNameTag = Global::getClientInstance()->getLocalPlayer()->getNametag()->c_str();

			if (playerNameTag.find("\n") != std::string::npos)
				playerNameTag = playerNameTag.substr(0, playerNameTag.find("\n"));

			playerNameTag = Utils::sanitize(playerNameTag);

			disconnectPacket->mMessage =
				"§c[Flareon] §7Kicking " + playerNameTag + ", " +
				std::to_string(size) + " check(s) flagged\n" + coloredFlags + "\n§r§7";

		}
	}

	Utils::CallFunc<void*, const float*, const float*, const float*, const std::shared_ptr<Packet>&>( // CallFunc to call the original.
		onDisconnectPacketDispatcher, a1, networkIdentifier, netEventCallback, packet
	);
}

class LoopbackPacketSenderHook : public FuncHook {
public:
	bool Initialize() override
	{
		auto LoopbackVTable = *(uintptr_t**)Global::getClientInstance()->getLoopbackPacketSender();

		std::shared_ptr<Packet> textPacket = MinecraftPackets::createPacket((int)PacketID::Text);

		std::shared_ptr<Packet> disconnectPacket = MinecraftPackets::createPacket((int)PacketID::Disconnect);

		if (!Utils::HookFunction((void*)LoopbackVTable[2], (void*)&LoopbackPacketSenderDetour, &onLoopbackPacketSender, xorstr_("LoopbackPacketSender"))) { return false; };

		if (!Utils::HookFunction((void*)textPacket->packetHandlerDispatcher->vTable[1], (void*)&TextPacketDispatcherDetour, &onTextPacketDispatcher, xorstr_("TextPacketDispatcher"))) { return false; };

		if (!Utils::HookFunction((void*)disconnectPacket->packetHandlerDispatcher->vTable[1], (void*)&DisconnectPacketDispatcherDetour, &onDisconnectPacketDispatcher, xorstr_("DisconnectPacketDispatcher"))) { return false; };

		return true;
	}

	static LoopbackPacketSenderHook& Instance() { // a class setup function called Instance.
		static LoopbackPacketSenderHook instance;
		return instance;
	}
};