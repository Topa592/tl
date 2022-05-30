#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include "tlBasicCodeEditor.h"
#include <chrono>
#include <thread>
#include "imgui.h"
#include "imgui_stdlib.h"

/*
* on autohotkey #SingleIstance Force
*/

namespace tl {
	namespace ahk {
		struct BaseVariable {
			std::string name = "";
			std::string value = "";
		};
		struct Variable {
			const std::string& name;
			std::string& value;
			Variable(BaseVariable& var)
				: name(var.name), value(var.value) {}
		};
		Variable GetVariableFromVector(
			const std::string& name,
			std::vector<BaseVariable>& list
		) {
			for (BaseVariable& var : list) {
				if (var.name != name) continue;
				return var;
			}
			throw std::runtime_error("Not Existing Variable name");
		}
		namespace parser {
			std::vector<BaseVariable> parseTextIntoVariables(
				const std::vector<std::string>& linesOfText
			) {
				std::vector<BaseVariable> list;
				for (const std::string& line : linesOfText) {
					BaseVariable var;
					int state = 0;
					for (const char& c : line) {
						switch (state)
						{
						case 0:
							switch (c)
							{
							case ' ':
							case ':':
								break;
							case '=':
								state = 1;
								break;
							default:
								var.name += c;
								break;
							}
							break;
						case 1:
							switch (c)
							{
							case ' ':
								break;
							case '\n':
								state = 2;
								break;
							default:
								var.value += c;
								break;
							}
							break;
						}
					}
					list.push_back(var);
				}
				return list;
			}
			std::vector<std::string> parseStringIntoLines(
				const std::string s
			) {
				std::vector<std::string> linesOfText;
				linesOfText.push_back("");
				for (int i = 0; i < s.size(); i++) {
					const char& c = s[i];
					if (c == '\n' && i != s.size() - 1) {
						linesOfText.push_back("");
					}
					else linesOfText.back().push_back(c);
				}
				return linesOfText;
			}
			std::string parseLinesIntoString(
				const std::vector<std::string>& linesOfText
			) {
				std::string s = "";
				for (const std::string& line : linesOfText) {
					s += line + '\n';
				}
				return s;
			}
			std::string parseBaseVariablesIntoString(
				const std::vector<BaseVariable>& baseVariables
			) {
				std::string s = "";
				for (const BaseVariable& var : baseVariables) {
					if (var.name != "") s += var.name + " := " + var.value;
					s += '\n';
				}
				return s;
			}
			std::vector<BaseVariable> parseStringIntoBaseVariables(
				const std::string& s
			) {
				return parseTextIntoVariables(parseStringIntoLines(s));
			}
		}
		namespace fileReaders {
			std::vector<std::string> fileAreaIntoStrings(
				const std::string& areaComment,
				const std::string& file
			) {
				std::fstream fs(file);
				std::string s = tl::bce::TextBetweenAreaComments(areaComment, fs);
				fs.close();
				return tl::ahk::parser::parseStringIntoLines(s);
			}
			std::string fileAreaIntoString(
				const std::string& areaComment,
				const std::string& file
			) {
				std::fstream fs(file);
				return tl::bce::TextBetweenAreaComments(areaComment, fs);
			}
		}
		
		void RunExec(const std::string& ahkScriptPath) {
			ShellExecuteA(NULL, NULL, ahkScriptPath.c_str(), NULL, NULL, 0);			
		}

		class SingleScriptRuntime {
		private:
			std::string scriptRelativePath;
			std::string script;
			std::string title;
			const std::string areaComment = ";tlahkruntime1";
			std::vector<BaseVariable> baseVariables;
			std::vector<BaseVariable> tlVariables;
			std::filesystem::file_time_type lastTimeThisUpdated = LastTimeModified();
			std::chrono::steady_clock::time_point lastTimeRanScript = std::chrono::steady_clock::now() - std::chrono::seconds(3);
			void ReOpenScript() {
				auto time = std::chrono::steady_clock::now();
				if (time - lastTimeRanScript < std::chrono::seconds(2)) std::this_thread::sleep_for(std::chrono::seconds(2));
				RunExec(script);
				lastTimeRanScript = std::chrono::steady_clock::now();
			}
			void StartUp() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "false";
			}
			void Shutdown() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "true";
				UpdateAll();
			}
			void Turnon() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "false";
				UpdateAll();
			}
			void Suspend() {
				Variable suspend = GetVariableFromVector("tl_suspend", tlVariables);
				suspend.value = (suspend.value == "true") ? "false" : "true";
				UpdateAll();
			}
			void ParseVariables(
				const std::vector<std::string>& linesOfText
			) {
				std::vector<BaseVariable> base;
				std::vector<BaseVariable> tl;
				for (const std::string& line : linesOfText) {
					BaseVariable var;
					int state = 0;
					for (const char& c : line) {
						switch (state)
						{
						case 0:
							switch (c)
							{
							case ' ':
							case ':':
								break;
							case '=':
								state = 1;
								break;
							default:
								var.name += c;
								break;
							}
							break;
						case 1:
							switch (c)
							{
							case ' ':
								break;
							case '\n':
								state = 2;
								break;
							default:
								var.value += c;
								break;
							}
							break;
						}
					}
					if (line.starts_with("tl_")) {
						tl.push_back(var);
					} else base.push_back(var);
				}
				baseVariables = base;
				tlVariables = tl;
			}
			void ReInitializeBaseVariables() {
				ParseVariables(
					tl::ahk::parser::parseStringIntoLines(
						tl::ahk::fileReaders::fileAreaIntoString(areaComment, script)
					)
				);
			}
			std::filesystem::file_time_type LastTimeModified() {
				return std::filesystem::last_write_time(script);
			}
			bool IfFileLastTimeUpdatedByThis() {
				return LastTimeModified() == lastTimeThisUpdated;
			}
		public:
			void DrawWindow() {
				ImGui::Begin(title.c_str());
				if (ImGui::Button("Reload")) UpdateAll();
				ImGui::SameLine();
				if (ImGui::Button("Start")) Turnon();
				ImGui::SameLine();
				if (ImGui::Button("Stop")) Shutdown();
				ImGui::SameLine();
				if (ImGui::Button("Suspend")) Suspend();
				ImGui::SameLine();
				if (ImGui::Button("Show tlvar")) ImGui::OpenPopup("tlvar_popup");
				if (ImGui::BeginPopup("tlvar_popup")) {
					for (BaseVariable& var : tlVariables) {
						ImGui::Text("%s := %s", var.name.c_str(), var.value.c_str());
					}
					ImGui::EndPopup();
				}
				for (BaseVariable& var : baseVariables) {
					if (var.name == "") {
						ImGui::Separator();
						continue;
					}
					else {
						ImGui::InputText(var.name.c_str(), &var.value);
					}
				}
				ImGui::End();
			}
			
			void UpdateAll() {
				std::string fullText = "";
				if (IfFileLastTimeUpdatedByThis()) {
					fullText += tl::ahk::parser::parseBaseVariablesIntoString(tlVariables);
					fullText += tl::ahk::parser::parseBaseVariablesIntoString(baseVariables);
					tl::bce::ReplaceWithText(
						areaComment,
						fullText,
						script
					);
				}
				else {
					ReInitializeBaseVariables();
				}
				lastTimeThisUpdated = LastTimeModified();
				ReOpenScript();
			}
			Variable GetVariable(std::string name) {
				return GetVariableFromVector(name, baseVariables);
			}
			std::vector<Variable> GetAllVariables() {
				std::vector<Variable> variables;
				for (BaseVariable& var : baseVariables) {
					variables.push_back(var);
				}
				return variables;
			}
			void CloseScript() {
				Shutdown();
				Sleep(3000); //TODO actually check that script is closed
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "false";
				std::string fullText = "";
				fullText += tl::ahk::parser::parseBaseVariablesIntoString(tlVariables);
				fullText += tl::ahk::parser::parseBaseVariablesIntoString(baseVariables);
				tl::bce::ReplaceWithText(
					areaComment,
					fullText,
					script
				);
			}
			SingleScriptRuntime(const std::string& scriptRelativePath)
				: scriptRelativePath(scriptRelativePath)
				, title(scriptRelativePath)
				, script(tl::bce::RelativeToDirectPath(scriptRelativePath))
			{
				tl::bce::validateFile(areaComment, script);
				ReInitializeBaseVariables();
				StartUp();
				UpdateAll();
			}
		};

		class ScriptHandler {
			std::vector<SingleScriptRuntime> scripts;
		public:
			void OpenScript(const std::string& ahkScriptRelativePath) {
				try
				{
					scripts.emplace_back(ahkScriptRelativePath);
				}
				catch (const std::exception& e)
				{
					scripts.pop_back();
					std::cout << e.what() << std::endl;
				}
			}
			void DrawAll() {
				for (SingleScriptRuntime& script : scripts) {
					script.DrawWindow();
				}
			}
			~ScriptHandler() {
				for (SingleScriptRuntime& script : scripts) {
					script.CloseScript();
				}
			}
		};
	}
}