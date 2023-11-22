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
		struct Action {
			std::string keybind;
			std::string actionName;
		};
		struct Macro {
			std::vector<std::string> lines;
			std::string fullString;
		};
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
		private:
			void AddKeyModifier(std::vector<std::string>& newLines, std::string modifierText) {
				std::string startText = "ControlSend, , {";
				std::string endText = "}, FINAL FANTASY XIV";
				std::string sleepText = "Sleep ";
				newLines.push_back(sleepText + "300");
				newLines.push_back(startText + modifierText + endText);
			}
			void MigrateNewKeybindToFunction(tl::ahk::BaseFunction& func, const std::vector<std::string>& oldKeybinds, const std::vector<std::string>& newKeybinds) {
				if (oldKeybinds.size() != newKeybinds.size()) throw "different sized keybinds";
				bool shiftDown = false;
				bool ctrlDown = false;
				bool altDown = false;
				std::vector<std::string> newActions = {};
				for (const std::string& s : func.linesOfValue) {
					if (s.find("Sleep") != std::string::npos) continue;
					std::string data = "";
					{
						std::string::size_type start = s.find('{') + 1; //first character
						std::string::size_type end = s.find('}') - 1; //last character
						if (start == std::string::npos || end == std::string::npos) throw "invalid autocraftlist";
						data = s.substr(start, end - start + 1);
					}
					if (data.find("Shift") != std::string::npos) {
						if (data.find("Down") != std::string::npos) {
							shiftDown = true;
						}
						else {
							shiftDown = false;
						}
						continue;
					}
					if (data.find("Ctrl") != std::string::npos) {
						if (data.find("Down") != std::string::npos) {
							ctrlDown = true;
						}
						else {
							ctrlDown = false;
						}
						continue;
					}
					if (data.find("Alt") != std::string::npos) {
						if (data.find("Down") != std::string::npos) {
							altDown = true;
						}
						else {
							altDown = false;
						}
						continue;
					}
					if (data.find("Up") != std::string::npos) {
						continue;
					}
					std::string key = "";
					if (shiftDown) key += "+";
					if (ctrlDown) key += "^";
					if (altDown) key += "!";
					key += data.front();
					for (int i = 0; i < oldKeybinds.size(); i++) {
						if (key == oldKeybinds[i]) {
							newActions.push_back(newKeybinds[i]);
							break;
						}
						if (i == oldKeybinds.size() - 1) {
							throw "invalid oldkeybind in autocraftlist";
						}
					}
				}

				int timeBetweenFfxivActions = 2500;
				int timeBetweenKeys = 300;
				int curSleep = 0;
				std::string startText = "ControlSend, , {";
				std::string endText = "}, FINAL FANTASY XIV";
				std::string sleepText = "Sleep ";
				std::vector<std::string> newLines = {};
				shiftDown = false;
				ctrlDown = false;
				altDown = false;
				for (const std::string& cur : newActions) {
					curSleep = 0;
					if (cur.find('+') != std::string::npos) {
						if (!shiftDown) {
							curSleep += timeBetweenKeys;
							AddKeyModifier(newLines, "Shift Down");
							shiftDown = true;
						}
					}
					else {
						if (shiftDown) {
							curSleep += timeBetweenKeys;
							AddKeyModifier(newLines, "Shift Up");
							shiftDown = false;
						}
					}
					if (cur.find('^') != std::string::npos) {
						if (!ctrlDown) {
							curSleep += timeBetweenKeys;
							AddKeyModifier(newLines, "Ctrl Down");
							ctrlDown = true;
						}
					}
					else {
						if (ctrlDown) {
							curSleep += timeBetweenKeys;
							AddKeyModifier(newLines, "Ctrl Up");
							ctrlDown = false;
						}
					}
					if (cur.find('!') != std::string::npos) {
						if (!altDown) {
							curSleep += timeBetweenKeys;
							AddKeyModifier(newLines, "Alt Down");
							altDown = true;
						}
					}
					else {
						if (altDown) {
							curSleep += timeBetweenKeys;
							AddKeyModifier(newLines, "Alt Up");
							altDown = false;
						}
					}
					newLines.push_back(sleepText + std::to_string(timeBetweenFfxivActions - curSleep));
					newLines.push_back(startText + cur.back() + " Down" + endText);
					newLines.push_back(sleepText + "300");
					newLines.push_back(startText + cur.back() + " Up" + endText);
				}
				if (shiftDown) AddKeyModifier(newLines, "Shift Up");
				if (ctrlDown) AddKeyModifier(newLines, "Ctrl Up");
				if (altDown) AddKeyModifier(newLines, "Alt Up");
				func.linesOfValue = newLines;
			}
			std::vector<std::string> GetCurrentKeybinds() {
				std::fstream fs("CraftingScriptKeyBindHistory.txt");
				std::string text = "";
				std::string line;
				for (; std::getline(fs, line);) {} //line becomes last line
				fs.close();
				line = line.substr(line.find('|') + 1);
				std::vector<std::string> keys = {};
				std::string curKey = "";
				size_t curIndex = 0;
				while (line.find('|', curIndex) != std::string::npos) {
					size_t endIndex = line.find('|', curIndex);
					keys.push_back(line.substr(curIndex, endIndex - curIndex));
					curIndex = endIndex + 1;
				}
				keys.push_back(line.substr(curIndex));
				return keys;
			}
			std::vector<std::string> GetCurrentKeybindNames() {
				return { "Crafting Log", "Basic Synthesis", "Basic Touch", "Master's Mend", "Hasty Touch", "Rapid Synthesis", "Observe", "Tricks of the Trade", "Waste Not", "Veneration", "Standard Touch", "Great Strides", "Innovation", "Final Appraisal", "Waste Not II", "Byregot's Blessing", "Precise Touch", "Muscle Memory", "Careful Synthesis", "Manipulation", "Prudent Touch", "Focused Synthesis", "Focused Touch", "Reflect", "Preparatory Touch", "Groundwork", "Delicate Synthesis", "Intensive Synthesis", "Trained Eye", "Advanced Touch", "Prudent Synthesis", "Trained Finesse" };
			}
			bool checkForModifier(bool& shiftDown, bool& ctrlDown, bool& altDown, std::string s) {
				if (s.find("Shift Down") != std::string::npos) shiftDown = true;
				else if (s.find("Shift Up") != std::string::npos) shiftDown = false;
				else if (s.find("Ctrl Down") != std::string::npos) ctrlDown = true;
				else if (s.find("Ctrl Up") != std::string::npos) ctrlDown = false;
				else if (s.find("Alt Down") != std::string::npos) altDown = true;
				else if (s.find("Alt Up") != std::string::npos) altDown = false;
				else {
					return false;
				}
				return true;
			}
			std::vector<tl::ffxiv::Action> GetListOfActions() {
				const std::vector<std::string> names = GetCurrentKeybindNames();
				const std::vector<std::string> keybinds = GetCurrentKeybinds();
				std::vector<tl::ffxiv::Action> actions;
				if (names.size() != keybinds.size()) throw "Autocraftlist keybindNames and keybinds sizes are not same";
				for (int i = 0; i < names.size(); i++) {
					tl::ffxiv::Action a;
					a.actionName = names[i];
					a.keybind = keybinds[i];
					actions.push_back(a);
				}
				return actions;
			}
			std::string FindKeyFromString(std::string ahkLine) {
				std::string::size_type start = ahkLine.find("{") + 1;
				std::string::size_type end = ahkLine.find(" ", start);
				return ahkLine.substr(start, end-start);
			}
			bool IfIsTheKey(bool shiftDown, bool ctrlDown, bool altDown, std::string key, tl::ffxiv::Action action) {
				if (action.keybind.find(key) == std::string::npos) return false;
				bool aShift = (action.keybind.find('+') != std::string::npos);
				if (shiftDown xor aShift) return false;
				bool aCtrl = (action.keybind.find('^') != std::string::npos);
				if (ctrlDown xor aCtrl) return false;
				bool aAlt = (action.keybind.find('!') != std::string::npos);
				if (altDown xor aAlt) return false;
				return true;
			}
			std::vector<tl::ffxiv::Action> GetFunctionActions(int i) {
				std::vector<tl::ffxiv::Action> result;
				std::vector<tl::ffxiv::Action> listOfActions = GetListOfActions();
				const std::vector<std::string>& linesOfFunc = functions[i].linesOfValue;
				bool shiftDown = false;
				bool ctrlDown = false;
				bool altDown = false;
				for (const std::string& s : linesOfFunc) {
					if (s.find("Sleep") != std::string::npos) continue;
					if (checkForModifier(shiftDown, ctrlDown, altDown, s)) continue;
					if (s.find("Up") != std::string::npos) continue;
					//only down actions left
					std::string key = FindKeyFromString(s);
					bool found = false;
					for (tl::ffxiv::Action& a : listOfActions) {
						if (IfIsTheKey(shiftDown, ctrlDown, altDown, key, a)) {
							result.push_back(a);
							found = true;
							break;
						}
					}
					if (!found) {
						return {};
					}
				}
				return result;
			}
			void AddNewKeybindToHistory(const std::vector<std::string>& newKeybinds) {
				std::fstream fs("CraftingScriptKeyBindHistory.txt");
				std::string fullText = "";
				std::string lastLine;
				for (std::string line; std::getline(fs, line);) {
					fullText += line + "\n";
					lastLine = line;
				}
				fs.close();
				size_t pos = lastLine.find('|');
				int count = std::stoi(lastLine.substr(0, pos));
				count++;
				fullText += std::to_string(count);
				for (std::string key : newKeybinds) {
					fullText += "|" + key;
				}
				fs.open("CraftingScriptKeyBindHistory.txt", std::ios::out);
				fs << fullText;
				fs.close();
			}
			void MigrateAllFunctions(const std::vector<std::string>& newKeybinds) {
				std::vector<std::string> oldKeybinds = GetCurrentKeybinds();
				for (int i = 1; i < functions.size(); i++) {
					MigrateNewKeybindToFunction(functions[i], oldKeybinds, newKeybinds);
				}
			}
			void DrawKeybinds() {
				static std::vector<std::string> allKeybindNames = this->GetCurrentKeybindNames();
				static std::vector<std::string> allKeybinds = this->GetCurrentKeybinds();
				if (allKeybindNames.size() != allKeybinds.size()) {
					ImGui::Text("Invalid keybind data");
					return;
				}
				if (ImGui::Button("Save and Migrate##AutoCraftListKeybindsSave")) {
					MigrateAllFunctions(allKeybinds);
					AddNewKeybindToHistory(allKeybinds);
					this->JustUpdateAll();
				}
				if (ImGui::BeginListBox("##AutocraftListKeybinds")) {
					ImGui::PushItemWidth(30);
					for (int i = 0; i < allKeybindNames.size(); i++) {
						ImGui::PushID(i);
						ImGui::InputText(allKeybindNames[i].c_str(), &allKeybinds[i]);
						ImGui::PopID();
					}
					ImGui::PopItemWidth();
					ImGui::EndListBox();
				}
			}
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
				this->UpdateAndRun();
			}
			void DrawNewMacroWindowWhenOpen(bool setup = false, bool setupOpen = false, std::vector<tl::ffxiv::Macro> setupMacros = {}) {
				static bool open = false;
				static std::vector<tl::ffxiv::Macro> macros;
				if (setup) {
					macros = setupMacros;
					open = setupOpen;
					return;
				}
				if (!open) return;
				ImGui::Begin("MacroWindow##MacroCraft", &open);
				//TODO Craft on MacroCraft window
				//ImGui::Button("Craft##MacroCraft") {}
				for (int i = 0; i < macros.size(); i++) {
					std::string name = R"(Macro)" + std::to_string(i+1) + "##MacroCraft";
					ImGui::InputTextMultiline(name.c_str(), &macros[i].fullString, {250, 190});
				}
				ImGui::End();
			}
			void RunMacroCraft(int index) {
				std::vector<tl::ffxiv::Action> actions = this->GetFunctionActions(index);
				std::vector<tl::ffxiv::Macro> macros;
				tl::ffxiv::Macro curMacro;
				int count = 1;
				for (Action a : actions) {
					if (count == 15) {
						macros.push_back(curMacro);
						curMacro.lines.clear();
						count = 1;
					}
					curMacro.lines.push_back(R"(/ac ")" + a.actionName + R"(" <wait.3>)");
					count++;
				}
				macros.push_back(curMacro);
				for (auto& m : macros) {
					for (const auto& s : m.lines) {
						m.fullString += s + "\n";
					}
					m.fullString.pop_back();
				}
				DrawNewMacroWindowWhenOpen(true, true, macros);
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
				if (ImGui::Button("MacroCraft##Autocraftlist")) {
					if (currentSelected != -1) RunMacroCraft(currentSelected);
				}
				DrawNewMacroWindowWhenOpen();
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
				ImGui::BeginListBox("##AutocraftListList");
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
				if (ImGui::TreeNode("Keybinds##Autocraftlist")) {
					DrawKeybinds();
					ImGui::TreePop();
				}
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
				UpdateAndRun();
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
				UpdateAndRun();
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
				if (ImGui::Button("Save##directmacro")) {
					directWindowOpen = false;
					macro = directMacroText;
					directMacroText = "";
					UpdateMacro();
				}
				ImGui::SameLine();
				if (ImGui::Button("Close##directmacro")) {
					directWindowOpen = false;
					directMacroText = "";
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