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
		enum class VariableType {
			normal,
			tl,
			trueFalse,
			lineChange,
			comment,
			Dropbox,
		};
		struct BaseVariable {
			std::string name = "";
			std::string value = "";
			VariableType type = VariableType::normal;
			std::string toString() const {
				switch (type)
				{
				case tl::ahk::VariableType::normal:
				case tl::ahk::VariableType::tl:
				case tl::ahk::VariableType::trueFalse:
					return name + " := " + value;
				case tl::ahk::VariableType::lineChange:
					return "";
				case tl::ahk::VariableType::comment:
					return value;
				case tl::ahk::VariableType::Dropbox:
					return ";Dropbox " + name + " " + value;
				default:
					return "";
				}
			}
		};
		struct Variable {
			const std::string& name;
			std::string& value;
			VariableType type;
			Variable(BaseVariable& var)
				: name(var.name), value(var.value), type(var.type) {}
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
					s += var.toString() + '\n';
				}
				return s;
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
		protected:
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
					if (line.size() >= 1) {
						if (line[0] == ';') {
							var.type = VariableType::comment;
							int state = 0;
							if (line.starts_with(";Dropbox ")) {
								var.type = VariableType::Dropbox;
								std::string::size_type start = std::string(";Dropbox ").size();
								std::string::size_type end = line.find(' ', start);
								var.name = line.substr(start, end);
								if (end != std::string::npos) var.value = line.substr(end + 1);
							}
							else {
								var.value = line;
							}
						}
						else {
							var.type = VariableType::normal;
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
							if (var.value == "true" || var.value == "false") {
								var.type = VariableType::trueFalse;
							}
						}
					}
					else {
						var.type = VariableType::lineChange;
					}

					if (line.starts_with("tl_")) {
						var.type = VariableType::tl;
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
			void Shutdown() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "true";
				UpdateAll();
			}
			void DrawToolbar() {
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
			}
			void DrawVariables() {
				bool InsideDropbox = false;
				bool DropboxOn = false;
				for (BaseVariable& var : baseVariables) {
					if (var.type == VariableType::lineChange || var.type == VariableType::Dropbox) {
						InsideDropbox = false;
						DropboxOn = false;
					}
					if (DropboxOn == false && InsideDropbox == true) {
						continue;
					}
					switch (var.type)
					{
					case VariableType::normal:
						ImGui::InputText(var.name.c_str(), &var.value);
						break;
					case VariableType::lineChange:
						InsideDropbox = false;
						DropboxOn = false;
						ImGui::Separator();
						break;
					case VariableType::trueFalse:
						if (var.value == "true") {
							std::string label = "True##" + var.name;
							if (ImGui::Button(label.c_str())) {
								var.value = "false";
							}
						}
						else {
							std::string label = "False##" + var.name;
							if (ImGui::Button(label.c_str())) {
								var.value = "true";
							}
						}
						ImGui::SameLine();
						ImGui::Text(var.name.c_str());
						break;
					case VariableType::Dropbox:
						if (ImGui::CollapsingHeader(var.name.c_str())) DropboxOn = true;
						InsideDropbox = true;
						break;
					default:
						break;
					}
				}
			}
			virtual void DrawWindow() {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				DrawVariables();
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
			void FixStartStateOutsideProgram() {
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
			SingleScriptRuntime(const std::string& title, const std::string& scriptDirectPath)
				: title(title)
				, script(scriptDirectPath)
			{
				tl::bce::validateFile(areaComment, script);
				ReInitializeBaseVariables();
				StartUp();
				UpdateAll();
			}
		};

		class ScriptHandler {
			std::vector<SingleScriptRuntime> scripts;
			std::vector<SingleScriptRuntime*> outsideScripts;
			
		public:
			void OpenScript(const std::string& title, const std::string& ahkScriptDirectPath) {
				try
				{
					scripts.emplace_back(title, ahkScriptDirectPath);
				}
				catch (const std::exception& e)
				{
					std::cout << e.what() << std::endl;
				}
			}
			void OpenScriptRelativePath(const std::string& ahkScriptRelativePath) {
				OpenScript(ahkScriptRelativePath, tl::bce::RelativeToDirectPath(ahkScriptRelativePath));
			}
			void OpenAllScriptsInFolder(const std::string& folderPath) {
				namespace fs = std::filesystem;
				for (auto& p : fs::directory_iterator(folderPath)) {
					std::string s = p.path().generic_string();
					if (s.ends_with(".ahk")) {
						OpenScript(fs::relative(p.path(), folderPath).generic_string(), s);
					}
				}
			}
			void OpenAllScriptsInFolder() {
				std::string thisPath = tl::bce::RelativeToDirectPath("");
				OpenAllScriptsInFolder(thisPath);
			}
			void AddScript(tl::ahk::SingleScriptRuntime& script) {
				outsideScripts.push_back(&script);
			}
			void DrawAll() {
				for (SingleScriptRuntime& script : scripts) {
					script.DrawWindow();
				}
				for (SingleScriptRuntime* script : outsideScripts) {
					script->DrawWindow();
				}
			}
			~ScriptHandler() {
				for (SingleScriptRuntime& script : scripts) {
					script.Shutdown();
				}
				for (SingleScriptRuntime* script : outsideScripts) {
					script->Shutdown();
				}
				Sleep(3000);
				for (SingleScriptRuntime& script : scripts) {
					script.FixStartStateOutsideProgram();
				}
				for (SingleScriptRuntime* script : outsideScripts) {
					script->FixStartStateOutsideProgram();
				}
			}
		};
	}
}