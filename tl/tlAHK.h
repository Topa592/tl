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
			static bool firstTime = true;
			auto time = std::chrono::steady_clock::now();
			static auto time2 = std::chrono::steady_clock::now();
			if (!firstTime && time - time2 < std::chrono::seconds(2)) std::this_thread::sleep_for(std::chrono::seconds(2));
			ShellExecuteA(NULL, NULL, ahkScriptPath.c_str(), NULL, NULL, 0);			
			time2 = std::chrono::steady_clock::now();
			firstTime = false;
		}
		class SingleScriptRuntime {
		private:
			const std::string baseFile;
			const std::string script;
			const std::string areaComment = ";tlahkruntime1";
			std::vector<BaseVariable> baseVariables;
			void InitializeBaseVariables() {
				baseVariables = tl::ahk::parser::parseStringIntoBaseVariables(
					tl::ahk::fileReaders::fileAreaIntoString(areaComment, baseFile)
				);
			}
		public:
			void DrawWindow() {
				ImGui::Begin("Crafting script");
				if (ImGui::Button("Reload")) UpdateAll();
				for (BaseVariable& var : baseVariables) {
					ImGui::InputText(var.name.c_str(), &var.value);
				}
				ImGui::End();
			}
			void UpdateAll() {
				tl::bce::ReplaceWithText(
					areaComment,
					tl::ahk::parser::parseBaseVariablesIntoString(baseVariables),
					baseFile
				);
				tl::bce::RunEditor(areaComment, baseFile, script);
				RunExec(script);
			}
			Variable GetVariable(std::string name) {
				for (BaseVariable& var : baseVariables) {
					if (var.name != name) continue;
					return var;
				}
				throw std::runtime_error("Not Existing Variable name");
			}
			std::vector<Variable> GetAllVariables() {
				std::vector<Variable> variables;
				for (BaseVariable& var : baseVariables) {
					variables.push_back(var);
				}
				return variables;
			}
			SingleScriptRuntime(const std::string& baseFileRelativePath, const std::string& scriptRelativePath)
				: baseFile(tl::bce::RelativeToDirectPath(baseFileRelativePath))
				, script(tl::bce::RelativeToDirectPath(scriptRelativePath)) {
				tl::bce::validateFiles(areaComment, baseFile, script);
				InitializeBaseVariables();
				RunExec(script);
			}
		};

		class SingleScriptRuntime2 {
		private:
			const std::string script;
			const std::string areaComment = ";tlahkruntime1";
			std::vector<BaseVariable> baseVariables;
			std::vector<BaseVariable> tlVariables;
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
			void ParseVariables(
				const std::vector<std::string>& linesOfText
			) {
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
						tlVariables.push_back(var);
					} else baseVariables.push_back(var);
				}
			}
			void InitializeBaseVariables() {
				ParseVariables(
					tl::ahk::parser::parseStringIntoLines(
						tl::ahk::fileReaders::fileAreaIntoString(areaComment, script)
					)
				);
			}
		public:
			void DrawWindow() {
				ImGui::Begin("Crafting script");
				if (ImGui::Button("Reload")) UpdateAll();
				ImGui::SameLine();
				{
					Variable tl_shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
					if (tl_shutdown.value == "true") {
						if (ImGui::Button("Start")) {
							Turnon();
						}
					}
					else {
						if (ImGui::Button("Stop")) {
							Shutdown();
						}
					}
				}
				
				ImGui::SameLine();
				if (ImGui::Button("Suspend")) {
					Shutdown();
				}
				ImGui::SameLine();
				if (ImGui::Button("Show tlvar")) ImGui::OpenPopup("tlvar_popup");
				if (ImGui::BeginPopup("tlvar_popup")) {
					for (BaseVariable& var : tlVariables) {
						ImGui::Text("%s := %s", var.name.c_str(), var.value.c_str());
					}
					ImGui::EndPopup();
				}
				for (BaseVariable& var : baseVariables) {
					if (var.value == "") {
						if (var.name == "") {
							ImGui::Separator();
							continue;
						}
						ImGui::Text(var.name.c_str());
					}
					else ImGui::InputText(var.name.c_str(), &var.value);
				}
				ImGui::End();
			}
			void UpdateAll() {
				std::string fullText = "";
				fullText += tl::ahk::parser::parseBaseVariablesIntoString(tlVariables);
				fullText += tl::ahk::parser::parseBaseVariablesIntoString(baseVariables);
				tl::bce::ReplaceWithText(
					areaComment,
					fullText,
					script
				);
				RunExec(script);
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
			SingleScriptRuntime2(const std::string& scriptRelativePath)
				: script(tl::bce::RelativeToDirectPath(scriptRelativePath)) {
				tl::bce::validateFile(areaComment, script);
				InitializeBaseVariables();
				StartUp();
				UpdateAll();
			}
			~SingleScriptRuntime2() {
				Shutdown();
			}
		};
	}
}