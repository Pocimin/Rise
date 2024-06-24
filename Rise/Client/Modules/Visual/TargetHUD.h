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

	bool track3d = false;
	float rounding = 5;

	bool showCalculatedHearts = true;
	Vector3<float> targetPos;

	//static inline float targetHealth = 20.f;
	float targetHealth = 20.f;

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

	struct Distance { // A method used to sort between distances
		bool operator()(Actor* target, Actor* target2) {
			auto instance = Global::getClientInstance();
			auto localPlayer = instance->getLocalPlayer();
			return (target->getStateVector()->Position).distance(localPlayer->getStateVector()->Position) <= (target2->getStateVector()->Position).distance(localPlayer->getStateVector()->Position);
		}
	};

	std::vector<Actor*> AvailableTargetList;
	void CreateOurTargetList() {
		auto instance = Global::getClientInstance();
		auto localPlayer = instance->getLocalPlayer();
		auto list = localPlayer->getLevel()->getRuntimeActorList();
		auto lpPos = localPlayer->getStateVector()->Position;
		for (Actor* actor : list) {
			if (actor != localPlayer && actor->isAlive() && !actor->isBot()) { //  && !actor->isBot()
				float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
				if (dist < 7) {
					AvailableTargetList.push_back(actor);
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

		//sort based on distance
		AvailableTargetList.clear();
		CreateOurTargetList();

		static EasingUtil inEase;

		//sort based on distance
		std::sort(AvailableTargetList.begin(), AvailableTargetList.end(), Distance());

		(!AvailableTargetList.empty() && Global::getClientInstance()->getMinecraftGame()->CanUseKeys) ? inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 15.f / 10)
			: inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 15.f / 10);

		float inScale = inEase.easeOutExpo();

		if (inEase.isPercentageMax()) 
			inScale = 1;

		ImScaleStart();
		auto pos = Vector2<float>(ImRenderUtil::getScreenSize().x / 2 + 10, ImRenderUtil::getScreenSize().y / 2 + 10);
		auto rect = Vector4<float>(pos.x, pos.y, pos.x + 230, pos.y + 68);

		static std::string targetName;
		static bool shouldReset = false;

		int damageTime = 0;

		if (!AvailableTargetList.empty()) {
			auto target = AvailableTargetList[0];

			targetName = target->getNametag()->c_str();
			targetName = Utils::sanitize(targetName);
			targetName = targetName.substr(0, targetName.find('\n'));

			targetHealth = target->targetHealth;

			damageTime = target->getHurtTime();

			if (shouldReset) {
				target->targetHealth = 20.f;
				shouldReset = false;
			}

			if (target->getHurtTime() == 9) {
				target->targetHealth -= 0.5;
			}

			if (target->targetHealth <= 1) {
				//target->targetHealth += 4;
			}

			if (TimeUtils::hasTimeElapsed("HealthCalculate", 1000, true)) {
				if (target->targetHealth < 20) {
					if (targetHealth + 3 < 20) {
						target->targetHealth += 1;
					}
				}
			}

			if (target == nullptr)
				return;
		}
		else {
			shouldReset = true;
			//targetHealth = 0.f;
		}

		ImRenderUtil::fillRectangle(rect, UIColor(0, 0, 0), 0.6f, rounding);
		ImRenderUtil::fillShadowRectangle(rect, UIColor(0, 0, 0), 0.9f, 130.f, 0, rounding);

		static float DamageAnimation = damageTime * 0.7; //* 2
		DamageAnimation = Math::animate(damageTime * 0.7, DamageAnimation, ImRenderUtil::getDeltaTime() * 30.f);

		//Vector4<float> HeadPos = Vector4<float>(rect.x + 8, rect.y + 8, rect.x + 70, rect.y + 70);
		Vector4<float> HeadPos = Vector4<float>((rect.x + 8) + DamageAnimation, (rect.y + 8) + DamageAnimation, (rect.x + 60) - DamageAnimation, (rect.y + 60) - DamageAnimation);
		ImColor HeadColor = IM_COL32_WHITE;

		static ParticleEngine particleMgr;
		
		if (damageTime >= 1) {
			UIColor UIHeadColor = UIColor(255, 204, 203); //255, 114, 118
			HeadColor = ImColor(UIHeadColor.r, UIHeadColor.g, UIHeadColor.b, UIHeadColor.a);
			ImRenderUtil::fillShadowRectangle(HeadPos, UIHeadColor, 1 * (DamageAnimation / 10), 90.f * (DamageAnimation / 10), 0, rounding);
			//particleMgr.drawParticles();
			//particleMgr.updateParticles(ImRenderUtil::getDeltaTime() * 0.9f);
			//particleMgr.addParticles(5, HeadPos.getCenter().x, HeadPos.getCenter().y, 80, 2.f);
		}

		//ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 114, 118, 255), rounding);
		ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), HeadColor, rounding);

		ImRenderUtil::drawText(Vector2<float>(pos.x + 67, pos.y + 17), &targetName, UIColor(255, 255, 255), 1.2f, 1.f, true);
		//ImRenderUtil::drawText(Vector2<float>(pos.x + 75.5f, pos.y + 8), &targetName, UIColor(255, 255, 255), 1.4f, 1.f, true);

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(0) << targetHealth;

		std::string healthStr = oss.str() + (std::string)" HP";

		static float healthEase = targetHealth / 20.f;

		healthEase = Math::animate(targetHealth / 20.f, healthEase, ImRenderUtil::getDeltaTime() * 10.f);

		ImRenderUtil::drawText(Vector2<float>(pos.x + 67, pos.y + 41), &healthStr, UIColor(255, 255, 255), 1.f, 1.f, true);

		auto healthRect = Vector4<float>(pos.x + 67, pos.y + 30, pos.x + 75 + (150 * healthEase), pos.y + 39);
		auto healthFilledRect = Vector4<float>(pos.x + 67, pos.y + 30, pos.x + 75 + 150, pos.y + 39);

		/*ImRenderUtil::fillShadowRectangle(healthFilledRect, UIColor(9, 3, 2), 1.f, 20.f, 0, 70.f);
		ImRenderUtil::fillRectangle(healthFilledRect, UIColor(9, 3, 2), 1.f, 50.f);

		ImRenderUtil::fillShadowRectangle(healthRect, ColorUtils::Rainbow(2.5, 1, 1, 1), 1.f, 40.f, 0, 70.f);
		ImRenderUtil::fillRectangle(healthRect, ColorUtils::Rainbow(2.5, 1, 1, 1), 1.f, 50.f);
		ImRenderUtil::fillGradientOpaqueRectangle(healthRect, UIColor(0, 0, 0), UIColor(0, 0, 0), 0.f, 0.25f);*/

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