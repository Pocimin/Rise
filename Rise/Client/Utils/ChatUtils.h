#pragma once

class ChatUtils
{
public:

	static void sendMessage(std::string message)
	{
		Global::getClientInstance()->getLocalPlayer()->displayClientMessage(Utils::combine(DARK_GRAY, "[", GRAY, "Rise", DARK_GRAY, "] ", GRAY, BOLD, RESET, message));
	}

	static void sendNormalMessage(std::string message)
	{
		Global::getClientInstance()->getLocalPlayer()->displayClientMessage(message);
	}
};
