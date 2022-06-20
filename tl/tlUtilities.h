#pragma once
#include <imgui.h>
#include <imgui_stdlib.h>
#include <chrono>

namespace tl {
	//stands for utility
	namespace u {
		void DrawUtilityWindow() {
			static bool open = true;
			if (!open) return;
			ImGui::Begin("Util Window", &open);
			ImGui::Text("Framerate %.0f", ImGui::GetIO().Framerate);
			if (ImGui::Button("Hide Console")) {
				ShowWindow(GetConsoleWindow(), SW_HIDE);
			}
			ImGui::SameLine();
			if (ImGui::Button("Show Console")) {
				ShowWindow(GetConsoleWindow(), SW_SHOWDEFAULT);
			}
			ImGui::End();
		}
		class ConsistantSleep {
		private:
			std::chrono::high_resolution_clock::time_point t1;
			std::chrono::high_resolution_clock::time_point sleepExtra;
			std::chrono::duration<double> sleepedExtra = std::chrono::milliseconds(0);
			std::chrono::duration<double> toSleep = std::chrono::milliseconds(0);
		public:

			void Start() {
				t1 = std::chrono::high_resolution_clock::now();
			}

			void End(int msTotal) {
				using namespace std;
				sleepExtra = chrono::high_resolution_clock::now();
				auto sleepTime = chrono::milliseconds(msTotal);
				auto timebetweenStartAndEnd = chrono::duration_cast<chrono::duration<double>>
					(chrono::high_resolution_clock::now() - t1);

				toSleep = sleepTime - sleepedExtra - timebetweenStartAndEnd;
				this_thread::sleep_for(toSleep);

				if (toSleep.count() <= 0) toSleep = chrono::milliseconds(0);

				sleepedExtra = chrono::duration_cast<chrono::duration<double>>
					(chrono::high_resolution_clock::now() - sleepExtra) - toSleep;
			}
		};
	}
}