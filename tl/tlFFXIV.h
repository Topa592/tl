#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "tlAHK.h"

namespace tl {
	namespace ffxiv {
		class CraftMacro {
		private:
			std::vector<int> sleepTimes;
			std::vector<int> GetMacroSleepTime(const std::string& file) {
				if (!std::filesystem::exists(file)) {
					throw std::runtime_error("Macro File Doesn't Exist");
					return {};
				}
				std::ifstream fs(file);
				std::vector<int> sleepTimes = { 0 };
				for (std::string line; std::getline(fs, line);) {
					if (line == "") {
						sleepTimes.push_back(0);
						continue;
					}
					std::string::size_type loc = line.find_last_of("<wait.");
					std::string::size_type locEnd = line.find('>', loc);
					if (loc == std::string::npos || locEnd == std::string::npos) continue;
					loc++;
					std::string::size_type length = locEnd - loc;
					std::string number = line.substr(loc, length);
					int num = std::stoi(number);
					sleepTimes.back() += num;
				}
				return sleepTimes;
			}
		public:
			int MacroCount() {
				return static_cast<int>(sleepTimes.size());
			}
			const std::vector<int>& GetSleepTimes() {
				return sleepTimes;
			}
			CraftMacro(const std::string& file)
				: sleepTimes(GetMacroSleepTime(file))
			{

			}
			void operator=(const CraftMacro& macro) {
				sleepTimes = macro.sleepTimes;
			}
		};

		class CraftingScript : public tl::ahk::SingleScriptRuntime {
			tl::ffxiv::CraftMacro macro;
			std::string macroPath;
		public:
			CraftingScript(
				const std::string& title,
				const std::string& scriptDirectPath,
				const std::string& macroDirectPath
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath }
				, macro(macroDirectPath)
				, macroPath('"' + macroDirectPath + '"')
			{

			}
			void UpdateMacro() {
				std::string s = macroPath;
				s.pop_back();
				s = s.substr(1);
				//macro = CraftMacro(s.substr(1));
				ImGui::OpenPopup("temp");
				if (ImGui::BeginPopup("temp")) {
					ImGui::Text("%s", "abc");
					ImGui::EndPopup();
				}
			}

			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				ImGui::InputText("", &macroPath);
				if (ImGui::Button("UpdateMacro")) {
					UpdateMacro();
				}
				ImGui::SameLine();
				if (ImGui::Button("MacroPath")) ImGui::OpenPopup("macroPath_popup");
				if (ImGui::BeginPopup("macroPath_popup")) {
					ImGui::Text("%s", macroPath.c_str());
					ImGui::EndPopup();
				}
				
				DrawVariables();
				ImGui::End();
			}
		};
	}
}