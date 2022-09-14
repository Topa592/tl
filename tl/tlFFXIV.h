#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "tlAHK.h"
#include "tlInputRecorder.h"
#include "tlFilesystem.h"
#include <format>

namespace tl {
	namespace ffxiv {
		namespace keybinds {
			static char key_confirm = VK_NUMPAD0;
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
		
		class AutoCraftList : public tl::ahk::SingleScriptRuntime {
		public:
			const std::string prefix = "tl_savedCraft_";
			int craftCount = 1;
			AutoCraftList(
				const std::string& title,
				const std::string& scriptDirectPath
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath, false }
			{}
			void AddNewFunc(tl::ahk::BaseFunction func) {
				if (func.name == functions[0].name) {
					func.name = "Craft" + std::to_string(functions.size()) + "()";
				}
				functions.push_back(func);
			}
			void DeleteFunc(int index) {
				functions.erase(functions.begin() + index);
			}
			void RunCraft(int index) {
				tl::ahk::Variable crafts = this->GetVariable("craftCount");
				crafts.value = std::to_string(craftCount);
				auto& func = functions[index];
				tl::ahk::BaseFunction& craftFunc = functions[0];
				craftFunc.linesOfValue = func.linesOfValue;
				this->UpdateAll(); //also runs it
			}
			void DrawWindow() override {
				static int currentSelected = -1;
				static bool renaming = false;
				static bool reordering = false;
				if (!ImGui::CollapsingHeader("SavedCrafts##Autocraftlist")) return;
				ImGui::InputInt("Craft Count##Autocraftlist", &craftCount);
				if (ImGui::Button("Craft##Autocraftlist")) {
					if (currentSelected != -1) RunCraft(currentSelected);
				}
				ImGui::SameLine();
				if (ImGui::Button("Rename##Autocraftlist")) {
					reordering = false;
					if (renaming) {
						this->JustUpdateAll();
					}
					renaming = !renaming;
				}
				ImGui::SameLine();
				if (ImGui::Button("Reorder##AutocraftList")) {
					renaming = false;
					if (reordering) {
						this->JustUpdateAll();
					}
					reordering = !reordering;
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete##Autocraftlist")) {
					if (currentSelected != -1) {
						DeleteFunc(currentSelected);
						this->JustUpdateAll();
						currentSelected = -1;
					}
				}
				ImGui::BeginListBox("");
				for (int i = 1; i < functions.size(); i++) {
					if (renaming) {
						ImGui::PushID(i);
						ImGui::InputText("", &functions[i].name);
						ImGui::PopID();
						continue;
					}
					bool ifSelected = false;
					if (i == currentSelected) ifSelected = true;
					if (ImGui::Selectable(functions[i].name.c_str(), ifSelected)) {
						currentSelected = i;
					}
					if (reordering && ImGui::IsItemActive() && !ImGui::IsItemHovered())
					{
						tl::ahk::BaseFunction func = functions[i];
						int n_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
						if (n_next >= 1 && n_next < functions.size())
						{
							functions[i] = functions[n_next];
							functions[n_next] = func;
							currentSelected = n_next;
							ImGui::ResetMouseDragDelta();
						}
					}
				}
				ImGui::EndListBox();
			}
		};

		class AutoCraftScript : public tl::ahk::SingleScriptRuntime {
		public:
			std::vector<tl::ir::KeyInput> inputs;
			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				DrawVariables();
				ImGui::End();
			}
			bool ifUp(const tl::ir::KeyInput& key) {
				return (key.flags & LLKHF_UP);
			}
			std::string GetSend(const tl::ir::KeyInput& key) {
				//std::string lineSend = "  Send ";
				//lineSend += tl::ahk::windows::vkcodeToAHK(key);
				std::string lineSend = "  ControlSend, ,";
				lineSend += tl::ahk::windows::vkcodeToAHK(key);
				lineSend += ", FINAL FANTASY XIV";
				return lineSend;
			}
			std::string GetSleep(const tl::ir::KeyInput& previous, const tl::ir::KeyInput& cur) {
				std::string lineSleep = "  Sleep ";
				int sleepTime = cur.time - previous.time + 50;
				if (sleepTime < 300) sleepTime = 300;
				if (sleepTime > 2500) sleepTime = 2500;
				lineSleep += std::to_string(sleepTime);
				return lineSleep;
			}
			void UpdateFunction() {
				auto& lines = GetLinesOfFunction("Craft()");
				lines.clear();
				if (inputs.empty()) return;
				tl::ir::KeyInput previous = inputs.front();
				lines.push_back(GetSend(previous));
				for (int i = 1; i < inputs.size(); i++) {
					tl::ir::KeyInput& cur = inputs[i];
					lines.push_back(GetSleep(previous, cur));
					lines.push_back(GetSend(cur));
					previous = cur;
				}
			}
			void StartScript(std::string count) {
				tl::ahk::Variable craftCount = GetVariable("craftCount");
				craftCount.value = count;
				UpdateFunction();
				UpdateAll();
			}
			const tl::ahk::BaseFunction& GetRawAHKFuncText() {
				for (auto& func : functions) {
					if (func.name == "Craft()") return func;
				}
			}
			AutoCraftScript(
				const std::string& title,
				const std::string& scriptDirectPath
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath, false }
			{}
		};
		

		class CraftingScript : public tl::ahk::SingleScriptRuntime {
			tl::ffxiv::CraftMacro macro;
			AutoCraftScript craft;
			AutoCraftList craftList;
		public:
			CraftingScript(
				const std::string& title,
				const std::string& scriptDirectPath,
				const bool startUp = false
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath, startUp }
				, craft("Autocraft.ahk", tl::filesystem::GetFolderPath(scriptDirectPath) + "Autocraft.ahk")
				, craftList("AutoCraftList.ahk", tl::filesystem::GetFolderPath(scriptDirectPath) + "AutoCraftList.ahk")
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
				if (!ImGui::Begin("New Macro", &directWindowOpen)) {
					directMacroText = "";
					ImGui::End();
					return;
				}
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
				if (ImGui::Button("SaveCraftScript")) {

				}
				ImGui::InputTextMultiline("", &directMacroText);
				ImGui::End();
			}


			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				ImGui::Separator();
				if (ImGui::Button("New Macro")) directWindowOpen = true;
				if (directWindowOpen) {
					DirectMacro();
				}
				if (ImGui::Button("startRecording")) {
					tl::ir::InputRecorder::StartRecording();
				}
				if (ImGui::Button("stopRecording")) {
					tl::ir::InputRecorder::StopRecording();
					craft.inputs = tl::ir::InputRecorder::GetRecording();
					Sleep(50);
				}
				static int playbackCount = 1;
				ImGui::InputInt("PlaybackCount", &playbackCount);
				if (ImGui::Button("StartCraftScript")) {
					craft.StartScript(std::to_string(playbackCount));
				}
				if (ImGui::Button("SaveRecording")) {
					craft.UpdateFunction();
					craftList.AddNewFunc(craft.GetFunction("Craft()"));
					craftList.JustUpdateAll();
				}
				craftList.DrawWindow();
				ImGui::Separator();
				DrawVariables();
				ImGui::End();
			}
		};

		class GatheringScript : public tl::ahk::SingleScriptRuntime {
		public:
			GatheringScript(
				const std::string& title,
				const std::string& scriptDirectPath,
				const bool startUp = false
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath, startUp }
			{}
			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();

				DrawVariables();
				ImGui::End();
			}
		};
		
		class CombatScript : public tl::ahk::SingleScriptRuntime {
		public:
			void DrawRecording() {

			}
			CombatScript(
				const std::string& title,
				const std::string& scriptDirectPath,
				const bool startUp = false
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath, startUp }
			{}
			void DrawWindow() override {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				DrawRecording();
				DrawVariables();
				ImGui::End();
			}
		};
	}
}