#pragma once
#include <vector>
#include <regex>

class TargetHUD : public Module {
public:
	TargetHUD(int keybind = Keys::NONE, bool enabled = false)
		: Module("TargetHUD", "Visual", "Display Target", keybind, enabled)
	{
		addBool("Track3D", "Changes the targethud position to the targets", &track3d);
		//addBool("Heart Check", "Calcultes Absorption / HP for hive and other servers that have health checks", &showCalculatedHearts);
		addSlider("Rounding", "The rounding of the targethud", &rounding, 1, 30);
	}

private:
	bool track3d = false;
	float rounding = 5;

	bool showCalculatedHearts = true;
	Vector3<float> targetPos;

	//static inline float targetHealth = 20.f;
	int targetIndex = 0;
public:

	static void ImScaleStart()
	{
		scale_start_index = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
	}

	static inline int scale_start_index;

	static ImVec2 ImScaleCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

		const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
		for (int i = scale_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
	}

	static void ImScaleEnd(float scaleX, float scaleY, ImVec2 center = ImScaleCenter())
	{
		auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;

		for (int i = scale_start_index; i < buf.Size; i++)
		{
			ImVec2 pos = buf[i].pos - center;
			pos.x *= scaleX;
			pos.y *= scaleY;
			buf[i].pos = pos + center;
		}
	}

	std::vector<Actor*> targetList;
	void CreateOurTargetList() {
		auto instance = Global::getClientInstance();
		auto localPlayer = instance->getLocalPlayer();
		auto list = localPlayer->getLevel()->getRuntimeActorList();
		auto lpPos = localPlayer->getStateVector()->Position;
		for (Actor* actor : list) {
			if (actor != localPlayer && actor->isAlive() && !actor->isBot()){
				float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
				if (dist <= 4) {
					if (getModuleByName("teams")->isEnabled()) {
						if (!actor->isTeammate(localPlayer)) {
							targetList.push_back(actor);
						}
					}
					else {
						targetList.push_back(actor);
					}
				}
			}
		}
	}

	void onEvent(ImGuiRenderEvent* event) {
		if (getModuleByName("clickgui")->isEnabled()) return;

		auto player = Global::getClientInstance()->getLocalPlayer();
		if (!player)
		{
			return;
		}

		if (player->getAttribute(AttributeId::Health) == nullptr) {
			return;
		}

		//sort based on distance
		targetList.clear();
		CreateOurTargetList();

		static EasingUtil inEase;

		bool shouldDisplay = !targetList.empty() && Global::getClientInstance()->getMinecraftGame()->CanUseKeys;

		(shouldDisplay) ? inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 15.f / 10)
			: inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 15.f / 10);

		float inScale = shouldDisplay ? inEase.easeOutElastic() : inEase.easeOutBack();

		if (inEase.isPercentageMax())
			inScale = 1;

		ImScaleStart();
		auto pos = Vector2<float>(ImRenderUtil::getScreenSize().x / 2 + 140, ImRenderUtil::getScreenSize().y / 2);
		auto rect = Vector4<float>(pos.x, pos.y, pos.x + 250, pos.y + 80);

		static std::string targetName;
		static std::string targetHealthStr;
		float targetHealth = 0.f;
		static float targetAbsorption = 0;
		static bool calculateHealth = false;
		int damageTime = 0;

		if (!targetList.empty()) {
			if (TimeUtils::hasTimeElapsed("TargetHudIndex", 300, true)) {
				targetIndex++;
			}

			if (targetIndex >= targetList.size())
				targetIndex = 0;

			auto target = targetList[targetIndex];
			if (target->getentityIdString().empty()) return;
			damageTime = target->getHurtTime();

			targetName = target->getNametag()->c_str();
			targetName = Utils::sanitize(targetName);
			targetName = "Name: " + targetName.substr(0, targetName.find('\n'));

			if (target->getHealth() == 20.f) {
				calculateHealth = true;
				targetHealth = target->getHealth(); // Later
			}
			else {
				calculateHealth = false;
				targetHealth = target->getHealth();
			}

			targetAbsorption = target->getAbsorption();

			std::ostringstream oss;
			oss << std::fixed << std::setprecision(1) << ((targetHealth + targetAbsorption) / 2);
			targetHealthStr = "Health: " + oss.str();

			if (target == nullptr)
				return;
		}
		else {
			targetIndex = 0;
		}

		if (inScale == 1) { ImRenderUtil::Blur(rect, 5, 14.f); }

		ImRenderUtil::fillRectangle(rect, UIColor(0, 0, 0), 0.6f, 14.f);
		ImRenderUtil::fillShadowRectangle(rect, UIColor(0, 0, 0), 0.8f, 50.f, 0, 14.f);

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
		ImRenderUtil::drawText(Vector2<float>(pos.x + 75, pos.y + 23), &targetName, UIColor(255, 255, 255), 1.2f, 1.f, true);
		ImGui::PopFont();

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
		ImRenderUtil::drawText(Vector2<float>(pos.x + 75, pos.y + 43), &targetHealthStr, UIColor(170, 170, 170), 1.f, 1.f, true);
		ImGui::PopFont();

		static float DamageAnimation = damageTime * 1.5; //* 2
		DamageAnimation = Math::animate(damageTime * 1.5, DamageAnimation, ImRenderUtil::getDeltaTime() * 30.f);

		Vector4<float> HeadPos = Vector4<float>((rect.x + 8) + DamageAnimation, (rect.y + 12) + DamageAnimation, (rect.x + 65) - DamageAnimation, (rect.y + 69) - DamageAnimation);
		ImColor HeadColor = IM_COL32_WHITE;

		static ParticleEngine particleMgr;

		if (damageTime >= 1) {
			UIColor UIHeadColor = UIColor(255, 114, 118); //255, 114, 118
			HeadColor = ImColor(UIHeadColor.r, UIHeadColor.g, UIHeadColor.b, UIHeadColor.a);
			ImRenderUtil::fillShadowRectangle(HeadPos, UIHeadColor, 1 * (DamageAnimation / 10), 40.f * (DamageAnimation / 10), 0, 14.f);
		}

		ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), HeadColor, 14.f);
		ImScaleEnd(inScale, inScale, ImVec2(pos.x + 125, pos.y + 40));

		/*for (Actor* TargetPlayerList : AvailableTargetList) {
			if (TargetPlayerList != player && TargetPlayerList->isAlive() && !TargetPlayerList->isBot()) { //  && !actor->isBot()
				Vector3<float> targetPos = AvailableTargetList[0]->getStateVector()->Position;

				float size = 47.f;


				float playerDistance = player->getStateVector()->Position.distance(targetPos);

				if (playerDistance <= 7) {
					// Get the TargetName
					std::string targetName = AvailableTargetList[0]->getNametag()->c_str();

					targetName = Utils::sanitize(targetName);
					targetName = targetName.substr(0, targetName.find('\n'));

					ImRenderUtil::fillRectangle(rect, UIColor(0, 0, 0), 0.6f, rounding);
					ImRenderUtil::fillShadowRectangle(rect, UIColor(0, 0, 0), 0.8f, 30.f, 0, rounding + 10.f);

					Vector4<float> HeadPos = Vector4<float>((rect.x + 8) + (AvailableTargetList[0]->getHurtTime() * 2), (rect.y + 8) + (AvailableTargetList[0]->getHurtTime() * 2), (rect.x + 55) - (AvailableTargetList[0]->getHurtTime() * 2), (rect.y + 55) - (AvailableTargetList[0]->getHurtTime() * 2));
					if (AvailableTargetList[0]->getHurtTime() >= 1) {
						ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 114, 118, 255), rounding);
					}
					else {
						ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, rounding);
					}

					ImRenderUtil::drawText(Vector2<float>(rect.x + 70, rect.y + 21), &targetName, UIColor(255, 255, 255), 1.15f, 1.f, true);
				}
			}
		}*/
	}

	void onEnabled() override {}

private:
};