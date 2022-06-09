#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "tlAHK.h"

namespace tl {
	namespace ffxiv {
		//stands for teamcraft
		namespace tc {

		}
		class CraftMacro {
		private:
			std::vector<int> sleepTimes = {};
			int SleepTimeFromLine(const std::string& line) {
				int total = 0;
				std::string::size_type curStart = 0;
				while (true) {
					std::string::size_type loc = line.find("<wait.", curStart);
					std::string::size_type locEnd = line.find('>', curStart);
					if (loc == std::string::npos || locEnd == std::string::npos) break;
					loc += std::string("<wait.").size();
					std::string::size_type length = locEnd - loc;
					std::string number = line.substr(loc, length);
					total += std::stoi(number);
					curStart = locEnd + 1;
				}
				return total;
			}
			void updateSleepTimesByString(const std::string& fullTextOfMacro) {
				std::vector<std::string> linesOfText = tl::ahk::parser::parseStringIntoLines(fullTextOfMacro);
				std::vector<int> sleepTimes = { 0 };
				for (const std::string& line : linesOfText) {
					if (line == "") {
						sleepTimes.push_back(0);
						continue;
					}
					sleepTimes.back() += SleepTimeFromLine(line);
				}
				this->sleepTimes = sleepTimes;
			}
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
					sleepTimes.back() += SleepTimeFromLine(line);
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
			CraftMacro() {

			}
			void operator=(const CraftMacro& macro) {
				sleepTimes = macro.sleepTimes;
			}
			void operator=(const std::string& fullTextOfMacro) {
				updateSleepTimesByString(fullTextOfMacro);
			}
		};

		class CraftingScript : public tl::ahk::SingleScriptRuntime {
			tl::ffxiv::CraftMacro macro;
		public:
			CraftingScript(
				const std::string& title,
				const std::string& scriptDirectPath
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath }
			{}
			std::vector<tl::ahk::Variable> Find3Variables(const std::string& varName) {
				std::vector<tl::ahk::Variable> variables;
				for (int i = 1; i <= 3; i++) {
					variables.push_back(GetVariable(varName + std::to_string(i)));
				}
				return variables;
			}
			void UpdateMacro() {
				std::vector<tl::ahk::Variable> sleeps = Find3Variables("sleep_craft");
				tl::ahk::Variable macroCount = GetVariable("macro_count");
				auto newSleepTimes = macro.GetSleepTimes();
				for (int i = 0; i < newSleepTimes.size() && i < 3; i++) {
					sleeps[i].value = std::to_string(newSleepTimes[i]) + "000";
				}
				macroCount.value = std::to_string(newSleepTimes.size());
				UpdateAll();
			}

			bool directWindowOpen = false;
			std::string directMacroText = "";

			void DirectMacro() {
				ImGui::Begin("New Macro");
				ImGui::Text("Copy paste your ffxiv macro here\nand if you have multiple macros\njust add empty line between them");
				if (ImGui::Button("Save")) {
					directWindowOpen = false;
					macro = directMacroText;
					directMacroText = "";
					UpdateMacro();
				}
				ImGui::SameLine();
				if (ImGui::Button("Close")) {
					directWindowOpen = false;
					directMacroText = "";
				}
				ImGui::InputTextMultiline("", &directMacroText);
				ImGui::End();
			}

			

			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				if (ImGui::Button("New Macro")) directWindowOpen = true;
				if (directWindowOpen) {
					DirectMacro();
				}
				
				DrawVariables();
				ImGui::End();
			}
		};

		class GatheringScript : public tl::ahk::SingleScriptRuntime {
		public:
			GatheringScript(
				const std::string& title,
				const std::string& scriptDirectPath
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath }
			{}
			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();

				DrawVariables();
				ImGui::End();
			}
		};
	}
}