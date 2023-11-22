#pragma once
#include <imgui.h>
#include <vector>
#include <string>

namespace tl {
	namespace miniapps {
		class miniapp;
		std::vector<miniapp*> apps;
		class miniapp {
		public:
			miniapp() {
				apps.push_back(this);
			}
			bool open = false;
			std::string appName = "unnamed";
			virtual void Draw() = 0;
			void Button() {
				if (ImGui::Button(appName.c_str())) {
					this->open = true;
				}
			}
		};
		
		void DrawWindow() {
			for (miniapp* app : apps) {
				if (app->open) app->Draw();
			}
			static bool open = true;
			if (!open) return;
			ImGui::Begin("miniapps", &open);
			for (int i = 0; i < apps.size(); i++) {
				ImGui::PushID(i);
				apps[i]->Button();
				ImGui::PopID();
			}
			ImGui::End();
		}
	}
}