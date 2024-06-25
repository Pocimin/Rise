#pragma once

class Killsults : public Module
{
private:
	bool notification = true;
	bool sound = true;
	int mode = 0;

	bool mentionplayer = true;

public:
	Killsults(int keybind = Keys::NONE, bool enabled = false) :
		Module("Killsults", "Player", "Insults the people you have killed", keybind, enabled)
	{
		addEnum("Mode", "The mode of message", { "Shadow", "UwUSpeak" }, &mode);
		addBool("Notification", "Show kill notification", &notification);
		addBool("Sound", "Play kill sound", &sound);
		addBool("Mention Player", "Mention the player you kill in a @target", &mentionplayer);
	}

	std::string Shadow[17] = {
		"Awww !target!, did your regen not break fast enough?",
		"Haha !target!, was your speed too slow to run away?",
		"!target!, your aura rots are missing!",
		"!target!, go ahead and run .uninject. Thanks!",
		"!target! just got shattered to pieces by my aura. Config issue!",
		"!target!, go ahead and disable \"Timer\" for me.",
		"I heard Derp headless disables anticheat, !target!!",
		"!target! and let me guess, your client is about to crash?",
		"Did Onix get paid for the client you're using, !target!?",
		"[Flareon] !target! flagged Bhop-A",
		"[Flareon] !target! flagged Fly-A",
		"I can just tell you use Packet, !target!.",
		"I can just tell you use Vector, !target!.",
		"If your client wasn't so bad, !target!, you might have had a chance.",
		"Did onix update your client yet, !target!?",
		"Which japanese client are you using this time, !target!?",
		"Uh oh, !target! is using packet client."
	};


	void sendMessage(std::string str) {
		std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(9);
		auto* pkt = reinterpret_cast<TextPacket*>(packet.get());

		pkt->type = TextPacketType::CHAT;
		pkt->message = str;
		pkt->PlatformId = "";
		pkt->Localize = false;
		pkt->Xuid = "";
		pkt->author = "";

		Global::getClientInstance()->getLoopbackPacketSender()->sendToServer(pkt);
	}

	std::string getKillsult(std::string const& target) {
		int randomVal = 0;
		randomVal = rand() % 17;

		std::string insult = Shadow[randomVal];

		if (insult.find("!target!") != std::string::npos) {
			insult.replace(insult.find("!target!"), 8, "@" + target);
		}

		return insult;
	}

	void onEvent(ActorBaseTickEvent* event) override {
		auto player = Global::getClientInstance()->getLocalPlayer();
		if (player == nullptr) return;

		if (Global::Killed) {
			sendMessage(getKillsult(Global::KilledTarget));

			Global::Killed = false;
		}
	}
};