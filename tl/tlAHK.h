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
#include "tlInputRecorder.h"
#include "tlImgui.h"
#include "tlFilesystem.h"

/*
* on autohotkey #SingleIstance Force
*/

namespace tl {
	namespace ahk {
		namespace closingFixer {
			namespace impl {
				std::vector<std::string> scripts;
				void AddScriptToBeFixedAtEnd(std::string directPath) {
					scripts.push_back(directPath);
				}
				
				void FixAllScriptStartingStates() {
					Sleep(3000);
					for (std::string scriptPath : scripts) {
						std::fstream fs(scriptPath);
						std::string text = "";
						for (std::string line; std::getline(fs, line);) {
							if (line == "tl_shutdown := true") {
								line = "tl_shutdown := false";
							}
							text += line + '\n';
						}
						fs.close();
						fs.open(scriptPath, std::ios::out);
						fs << text;
						fs.close();
					}
				}
			}
			void AddScriptToBeFixedAtEnd(std::string directPath) {
				impl::AddScriptToBeFixedAtEnd(directPath);
			}
		}
		namespace parser {
			std::vector<std::string> parseStringIntoLines(const std::string);
		}
		namespace windows {
			bool ifUp(const tl::ir::KeyInput& key) {
				return (key.flags & LLKHF_UP);
			}
			std::string vkcodeToAHK(const tl::ir::KeyInput& key) {
				std::string sendLine = "{";
				switch (key.vkCode)
				{
				case VK_BACK:		sendLine += "Backspace"; break;
				case VK_TAB:		sendLine += "Tab"; break;
				case VK_SHIFT:		sendLine += "Shift"; break;
				case VK_CONTROL:	sendLine += "Ctrl"; break;
				case VK_MENU:		sendLine += "AppsKey"; break;
				case VK_PAUSE:		sendLine += "Pause"; break;
				case VK_CAPITAL:	sendLine += "CapsLock"; break;
				case VK_ESCAPE:		sendLine += "Esc"; break;
				case VK_SPACE:		sendLine += "Space"; break;
				case VK_PRIOR:		sendLine += "PgUp"; break;
				case VK_NEXT:		sendLine += "PgDn"; break;
				case VK_END:		sendLine += "End"; break;
				case VK_HOME:		sendLine += "Home"; break;
				case VK_LEFT:		sendLine += "Left"; break;
				case VK_UP:			sendLine += "Up"; break;
				case VK_RIGHT:		sendLine += "Right"; break;
				case VK_DOWN:		sendLine += "Down"; break;
				case VK_PRINT:		sendLine += "PrintScreen"; break;
				case VK_INSERT:		sendLine += "Insert"; break;
				case VK_DELETE:		sendLine += "Delete"; break;
				case '0':			sendLine += "0"; break;
				case '1':			sendLine += "1"; break;
				case '2':			sendLine += "2"; break;
				case '3':			sendLine += "3"; break;
				case '4':			sendLine += "4"; break;
				case '5':			sendLine += "5"; break;
				case '6':			sendLine += "6"; break;
				case '7':			sendLine += "7"; break;
				case '8':			sendLine += "8"; break;
				case '9':			sendLine += "9"; break;
				case 'A':			sendLine += "a"; break;
				case 'B':			sendLine += "b"; break;
				case 'C':			sendLine += "c"; break;
				case 'D':			sendLine += "d"; break;
				case 'E':			sendLine += "e"; break;
				case 'F':			sendLine += "f"; break;
				case 'G':			sendLine += "g"; break;
				case 'H':			sendLine += "h"; break;
				case 'I':			sendLine += "i"; break;
				case 'J':			sendLine += "j"; break;
				case 'K':			sendLine += "k"; break;
				case 'L':			sendLine += "l"; break;
				case 'M':			sendLine += "m"; break;
				case 'N':			sendLine += "n"; break;
				case 'O':			sendLine += "o"; break;
				case 'P':			sendLine += "p"; break;
				case 'Q':			sendLine += "q"; break;
				case 'R':			sendLine += "r"; break;
				case 'S':			sendLine += "s"; break;
				case 'T':			sendLine += "t"; break;
				case 'U':			sendLine += "u"; break;
				case 'V':			sendLine += "v"; break;
				case 'W':			sendLine += "w"; break;
				case 'X':			sendLine += "x"; break;
				case 'Y':			sendLine += "y"; break;
				case 'Z':			sendLine += "z"; break;
				case VK_LWIN:		sendLine += "LWin"; break;
				case VK_RWIN:		sendLine += "RWin"; break;
				case VK_NUMPAD0:	sendLine += "Numpad0"; break;
				case VK_NUMPAD1:	sendLine += "Numpad1"; break;
				case VK_NUMPAD2:	sendLine += "Numpad2"; break;
				case VK_NUMPAD3:	sendLine += "Numpad3"; break;
				case VK_NUMPAD4:	sendLine += "Numpad4"; break;
				case VK_NUMPAD5:	sendLine += "Numpad5"; break;
				case VK_NUMPAD6:	sendLine += "Numpad6"; break;
				case VK_NUMPAD7:	sendLine += "Numpad7"; break;
				case VK_NUMPAD8:	sendLine += "Numpad8"; break;
				case VK_NUMPAD9:	sendLine += "Numpad9"; break;
				case VK_MULTIPLY:	sendLine += "NumpadMult"; break;
				case VK_ADD:		sendLine += "NumpadAdd"; break;
				case VK_SUBTRACT:	sendLine += "NumpadSub"; break;
				case VK_DECIMAL:	sendLine += "NumpadDot"; break;
				case VK_DIVIDE:		sendLine += "NumpadDiv"; break;
				case VK_F1:			sendLine += "F1"; break;
				case VK_F2:			sendLine += "F2"; break;
				case VK_F3:			sendLine += "F3"; break;
				case VK_F4:			sendLine += "F4"; break;
				case VK_F5:			sendLine += "F5"; break;
				case VK_F6:			sendLine += "F6"; break;
				case VK_F7:			sendLine += "F7"; break;
				case VK_F8:			sendLine += "F8"; break;
				case VK_F9:			sendLine += "F9"; break;
				case VK_F10:		sendLine += "F10"; break;
				case VK_F11:		sendLine += "F11"; break;
				case VK_F12:		sendLine += "F12"; break;
				case VK_F13:		sendLine += "F13"; break;
				case VK_F14:		sendLine += "F14"; break;
				case VK_F15:		sendLine += "F15"; break;
				case VK_F16:		sendLine += "F16"; break;
				case VK_F17:		sendLine += "F17"; break;
				case VK_F18:		sendLine += "F18"; break;
				case VK_F19:		sendLine += "F19"; break;
				case VK_F20:		sendLine += "F20"; break;
				case VK_F21:		sendLine += "F21"; break;
				case VK_F22:		sendLine += "F22"; break;
				case VK_F23:		sendLine += "F23"; break;
				case VK_F24:		sendLine += "F24"; break;
				case VK_NUMLOCK:	sendLine += "Numlock"; break;
				case VK_SCROLL:		sendLine += "ScrollLock"; break;
				case VK_LSHIFT:		sendLine += "Shift"; break;
				case VK_RSHIFT:		sendLine += "Shift"; break;
				case VK_LCONTROL:	sendLine += "Control"; break;
				case VK_RCONTROL:	sendLine += "Control"; break;
				case VK_LMENU:		sendLine += "Appskey"; break;
				case VK_RMENU:		sendLine += "AppsKey"; break;
				}
				if (ifUp(key)) {
					sendLine += " Up}";
				}
				else {
					sendLine += " Down}";
				}
				return sendLine;
			}
		}
		enum class VariableType {
			normal,
			tl,
			trueFalse,
			lineChange,
			comment,
			Dropbox,
			TextStatic,
		};
		struct BaseVariable {
			std::string name = "";
			std::string value = "";
			VariableType type = VariableType::normal;
			void DrawBasic() {
				switch (type)
				{
				case VariableType::normal:
					ImGui::InputText(name.c_str(), &value);
					break;
				case VariableType::trueFalse:
					if (value == "true") {
						std::string label = "True##" + name;
						if (ImGui::Button(label.c_str())) {
							value = "false";
						}
					}
					else {
						std::string label = "False##" + name;
						if (ImGui::Button(label.c_str())) {
							value = "true";
						}
					}
					ImGui::SameLine();
					ImGui::Text(name.c_str());
					break;
				case VariableType::TextStatic:
					ImGui::Text(value.c_str());
					break;
				}
			}
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
					return ";Dropbox " + name;
				case tl::ahk::VariableType::TextStatic:
					return "/*\n" + value + "\n*/";
				default:
					return "";
				}
			}
			void fromString(const std::string& s) {
				if (this->type == VariableType::TextStatic) {
					value = s;
					return;
				}
				if (s.size() >= 1) {
					if (s[0] == ';') {
						type = VariableType::comment;
						int state = 0;
						if (s.starts_with(";Dropbox ")) {
							type = VariableType::Dropbox;
							std::string::size_type start = std::string(";Dropbox ").size();
							name = s.substr(start);
						}
						else {
							value = s;
						}
					}
					else {
						type = VariableType::normal;
						int state = 0;
						for (const char& c : s) {
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
									name += c;
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
									value += c;
									break;
								}
								break;
							}
						}
						if (value == "true" || value == "false") {
							type = VariableType::trueFalse;
						}
					}
				}
				else {
					type = VariableType::lineChange;
				}
			}
		};
		namespace drawing {
			void DrawBaseVariables(std::vector<BaseVariable>& list) {
				bool InsideDropbox = false;
				bool DropboxOn = false;
				for (BaseVariable& var : list) {
					if (var.type == VariableType::lineChange || var.type == VariableType::Dropbox) {
						InsideDropbox = false;
						DropboxOn = false;
					}
					if (DropboxOn == false && InsideDropbox == true) {
						continue;
					}
					switch (var.type)
					{
					case VariableType::lineChange:
						InsideDropbox = false;
						DropboxOn = false;
						ImGui::Separator();
						break;
					case VariableType::Dropbox:
						if (ImGui::CollapsingHeader(var.name.c_str())) DropboxOn = true;
						InsideDropbox = true;
						break;
					case VariableType::comment:
						break;
					default:
						var.DrawBasic();
						break;
					}
				}
			}
		}
		struct BaseFunction {
			std::string name = "";
			std::vector<std::string> linesOfValue = {};
			std::string toString() const {
				std::string s = ";fn\n"
					+ name + " {\n";
				for (const std::string& line : linesOfValue) {
					s += line + "\n";
				}
				s += "}";
				return s;
			}
			void fromString(const std::string& s) {
				std::vector<std::string> lines = parser::parseStringIntoLines(s);
				int i = 0;
				if (lines.front().starts_with(";fn")) i = 1;
				size_t end = lines[i].find(')');
				name = lines[i].substr(0, end + 1);
				i++;
				//-1 size ignoring last line of '}' 
				for (; i < lines.size(); i++) {
					if (lines[i].starts_with("}")) break;
					linesOfValue.push_back(lines[i]);
				}
			}
			bool windowOpen = false;
			void DrawBasic() {
				if (ImGui::Button(name.c_str())) {
					windowOpen = true;
				}
				if (windowOpen) {
					std::string s = name.c_str();
					s += "##window";
					ImGui::Begin(s.c_str(), &windowOpen);
					ImGui::Text(toString().c_str());
					ImGui::End();
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
		std::vector<std::string>& GetLinesOfFunctionFromVector(
			const std::string& name,
			std::vector<BaseFunction>& list
		) {
			for (BaseFunction& func : list) {
				if (func.name != name) continue;
				return func.linesOfValue;
			}
			throw std::runtime_error("Not Existing Function name");
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
			template<typename T>
			std::string parseVariablesIntoString(
				const std::vector<T>& variables
			) {
				std::string s = "";
				for (const T& var : variables) {
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
		public:
			const std::string scriptDirectPath;
			std::string title;
		protected:
			const std::string areaComment = ";tlahkruntime1";
			std::vector<BaseVariable> baseVariables;
			std::vector<BaseVariable> tlVariables;
			std::vector<BaseFunction> functions;
			std::filesystem::file_time_type lastTimeThisUpdated = LastTimeModified();
			std::chrono::steady_clock::time_point lastTimeRanScript = std::chrono::steady_clock::now() - std::chrono::seconds(3);
			void ReOpenScript() {
				auto time = std::chrono::steady_clock::now();
				if (time - lastTimeRanScript < std::chrono::seconds(2)) std::this_thread::sleep_for(std::chrono::seconds(2));
				RunExec(scriptDirectPath);
				lastTimeRanScript = std::chrono::steady_clock::now();
			}
			void StartUp() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "false";
			}
			void ParseVariables(
				const std::vector<std::string>& linesOfText
			) {
				std::vector<BaseVariable> base;
				std::vector<BaseVariable> tl;
				std::vector<BaseFunction> fn;
				
				for (int i = 0; i < linesOfText.size(); i++) {
					std::string line = linesOfText[i];
					if (line.starts_with(";fn")) {
						std::string tempFunc = "";
						while (!line.starts_with("}")) {
							tempFunc += line + "\n";
							i++;
							line = linesOfText[i];
						}
						tempFunc += line + "\n";
						BaseFunction func;
						func.fromString(tempFunc);
						fn.push_back(func);
						continue;
					}
					if (line.starts_with("/*")) {
						std::string s = "";
						i++;
						while (i < linesOfText.size()) {
							line = linesOfText[i];
							if (line.starts_with("*/")) {
								break;
							}
							s += line + "\n";
							i++;
						}
						s.pop_back(); //removes last linechange
						BaseVariable var;
						var.type = VariableType::TextStatic;
						var.fromString(s);
						base.push_back(var);
						continue;
					}
					BaseVariable var;
					var.fromString(line);

					if (var.name.starts_with("tl_")) {
						var.type = VariableType::tl;
						tl.push_back(var);
					}
					else base.push_back(var);
				}
				functions = fn;
				baseVariables = base;
				tlVariables = tl;
			}
			void ReInitializeBaseVariables() {
				ParseVariables(
					tl::ahk::parser::parseStringIntoLines(
						tl::ahk::fileReaders::fileAreaIntoString(areaComment, scriptDirectPath)
					)
				);
			}
			std::filesystem::file_time_type LastTimeModified() {
				return std::filesystem::last_write_time(scriptDirectPath);
			}
			bool IfFileLastTimeUpdatedByThis() {
				return LastTimeModified() == lastTimeThisUpdated;
			}
		public:
			std::vector<tl::ahk::BaseFunction>& GetBaseFuncListReference() {
				return functions;
			}
			void Reload() {
				UpdateAndRun();
			}
			void Turnon() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "false";
				UpdateAndRun();
			}
			void Suspend() {
				Variable suspend = GetVariableFromVector("tl_suspend", tlVariables);
				suspend.value = (suspend.value == "true") ? "false" : "true";
				UpdateAndRun();
			}
			void Shutdown() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "true";
				UpdateAndRun();
			}
			void DrawToolbar() {
				if (ImGui::Button("Reload")) UpdateAndRun();
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
				drawing::DrawBaseVariables(baseVariables);
			}
			virtual void DrawWindow() {
				ImGui::Begin(title.c_str());
				DrawToolbar();
				DrawVariables();
				ImGui::End();
			}
			std::string GetFullText() {
				std::string fullText = "";
				fullText += tl::ahk::parser::parseVariablesIntoString(tlVariables);
				fullText += tl::ahk::parser::parseVariablesIntoString(baseVariables);
				fullText += tl::ahk::parser::parseVariablesIntoString(functions);
				return fullText;
			}
			void JustUpdateAll() {
				if (IfFileLastTimeUpdatedByThis()) {
					tl::bce::ReplaceWithText(
						areaComment,
						GetFullText(),
						scriptDirectPath
					);
				}
				else {
					ReInitializeBaseVariables();
				}
				lastTimeThisUpdated = LastTimeModified();
			}
			void UpdateAndRun() {
				JustUpdateAll();
				ReOpenScript();
			}
			Variable GetVariable(std::string name) {
				return GetVariableFromVector(name, baseVariables);
			}
			std::vector<std::string>& GetLinesOfFunction(std::string name) {
				return GetLinesOfFunctionFromVector(name, functions);
			}
			std::vector<Variable> GetAllVariables() {
				std::vector<Variable> variables;
				for (BaseVariable& var : baseVariables) {
					variables.push_back(var);
				}
				return variables;
			}
			const tl::ahk::BaseFunction& GetFunction(std::string name) {
				for (const BaseFunction& func : functions) {
					if (func.name == name) return func;
				}
				throw std::runtime_error("Not Existing Variable name");
			}
			void FixStartStateOutsideProgram() {
				Variable shutdown = GetVariableFromVector("tl_shutdown", tlVariables);
				shutdown.value = "false";
				tl::bce::ReplaceWithText(
					areaComment,
					GetFullText(),
					scriptDirectPath
				);
			}
			SingleScriptRuntime(const std::string& title, const std::string& scriptDirectPath, bool startUp = true)
				: title(title)
				, scriptDirectPath(scriptDirectPath)
			{
				tl::bce::validateFile(areaComment, scriptDirectPath);
				ReInitializeBaseVariables();
				if (startUp) {
					StartUp();
					UpdateAndRun();
				}
			}
		};

		class ScriptHandler {
			std::vector<SingleScriptRuntime> scripts;
			std::vector<SingleScriptRuntime*> outsideScripts;
		public:
			void OpenScript(const std::string& title, const std::string& ahkScriptDirectPath) {
				try
				{
					scripts.emplace_back(title, ahkScriptDirectPath, false);
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
					std::string s = p.path().string();
					if (s.ends_with(".ahk")) {
						OpenScript(fs::relative(p.path(), folderPath).string(), s);
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
					tl::ahk::closingFixer::AddScriptToBeFixedAtEnd(script.scriptDirectPath);
				}
				for (SingleScriptRuntime* script : outsideScripts) {
					script->Shutdown();
					tl::ahk::closingFixer::AddScriptToBeFixedAtEnd(script->scriptDirectPath);
				}
			}
		};
		namespace QuickScripts {
			namespace impl {
				std::string folderPath;
				std::vector<SingleScriptRuntime> scripts;
				
				void OpenScript(const std::string& title, const std::string& ahkScriptDirectPath) {
					try
					{
						scripts.emplace_back(title, ahkScriptDirectPath, false);
					}
					catch (const std::exception& e)
					{
						std::cout << e.what() << std::endl;
					}
				}
				void OpenAllScriptsInFolder(const std::string& folderPath) {
					namespace fs = std::filesystem;
					for (auto& p : fs::directory_iterator(folderPath)) {
						std::string s = p.path().string();
						if (s.ends_with(".ahk")) {
							OpenScript(fs::relative(p.path(), folderPath).string(), s);
						}
					}
				}
				void OpenAllScriptsInFolder() {
					std::string thisPath = tl::bce::RelativeToDirectPath("tlQuickScripts");
					OpenAllScriptsInFolder(thisPath);
				}
				void CopyScript(std::string from, std::string to) {
					std::fstream base(from);
					std::string fullText = tl::bce::PlainFullText(base);
					base.close();
					std::fstream newscript(to);
					newscript << fullText;
					newscript.close();
				}
				void CloseScripts() {
					for (SingleScriptRuntime& script : scripts) {
						script.Shutdown();
						tl::ahk::closingFixer::AddScriptToBeFixedAtEnd(script.scriptDirectPath);
					}
				}
				void CreateNewScript() {
					std::string base = tl::bce::RelativeToDirectPath("tlQuickScripts\\tl_base.ahk");
					std::string baseName = "NewScript";
					std::string ending = ".ahk";
					int cur = 1;
					while (true) {
						int tempCur = cur;
						for (SingleScriptRuntime& script : scripts) {
							std::string name = tl::filesystem::GetFileNameFromPath(script.scriptDirectPath);
							if (name == (baseName + std::to_string(cur) + ending)) {
								cur++;
							}
						}
						if (tempCur == cur) break;
					}
					std::string newScriptPath = tl::bce::RelativeToDirectPath("tlQuickScripts\\" + baseName + std::to_string(cur) + ending);
					std::filesystem::copy_file(base, newScriptPath);
					OpenScript(baseName + std::to_string(cur) + ending, newScriptPath);
				}
				void DrawWindow() {
					ImGui::Begin("QuickScripts");
					static int selected = -1;
					static bool DrawingSelected = false;
					if (ImGui::Button("NewScript##QuickScripts")) {
						CreateNewScript();
					}
					ImGui::SameLine();
					if (ImGui::Button("Draw Window##QuickScripts")) {
						DrawingSelected = !DrawingSelected;
					}
					if (ImGui::Button("Reload##QuickScripts") && (selected != -1)) {
						scripts[selected].Reload();
					}
					ImGui::SameLine();
					if (ImGui::Button("Start##QuickScripts") && (selected != -1)) {
						scripts[selected].Turnon();
					}
					ImGui::SameLine();
					if (ImGui::Button("Stop##QuickScripts") && (selected != -1)) {
						scripts[selected].Shutdown();
					}
					ImGui::SameLine();
					if (ImGui::Button("Suspend##QuickScripts") && (selected != -1)) {
						scripts[selected].Suspend();
					}

					ImGui::BeginListBox("##QuickScriptsList");

					for (int i = 0; i < scripts.size(); i++) {
						bool ifSelected = false;
						if (i == selected) ifSelected = true;
						if (ImGui::Selectable(scripts[i].title.c_str(), ifSelected)) {
							selected = i;
							if (ifSelected == false) {
								DrawingSelected = false;
							}
						}
					}
					ImGui::EndListBox();
					if (selected != -1) {
						if (DrawingSelected) {
							scripts[selected].DrawWindow();
						}
					}
					
					ImGui::End();
				}
				//-------
				//Publics
				//-------
				void StartScripts() {
					static bool started = false;
					if (!started) {
						started = true;
						tl::ig::StaticWindowDrawer::AddWindow(&DrawWindow);
					}
					std::string thisPath = tl::bce::RelativeToDirectPath("");
					impl::folderPath = thisPath + "tlQuickScripts";
					impl::OpenAllScriptsInFolder(folderPath);
				}
			}
			void StartScripts() {
				impl::StartScripts();
				std::atexit(impl::CloseScripts);
			}
		}
		namespace impl {
			int exit = std::atexit(ahk::closingFixer::impl::FixAllScriptStartingStates);
		}
	}
}