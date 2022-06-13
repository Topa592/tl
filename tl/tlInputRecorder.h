#pragma once
#include <Windows.h>
#include <cstdlib>
#include <queue>
#include <iostream>

namespace tl {
	namespace ir {
		namespace impl {
			HHOOK KeyHookWnd;
			struct KeyInput {
				LRESULT lResult;
				LPARAM lParam;
				WPARAM wParam;
				UINT message;
				HWND hwnd;
			};
			
			class InputPlayer {
				std::queue<KeyInput> keys;
			public:
				void push(CWPRETSTRUCT data) {
					std::cout << data.wParam << std::endl;
				}
				void playback() {
					while (!keys.empty()) {
						auto key = keys.front();


						keys.pop();
					}
				}
			};
			InputPlayer inputs;
			LRESULT Hookproc(int code, WPARAM wParam, LPARAM lParam) {
				inputs.push((CWPRETSTRUCT)lParam);
				return CallNextHookEx(NULL, code, wParam, lParam);
			}
			void AtExit() {
				if (KeyHookWnd) UnhookWindowsHookEx(KeyHookWnd);
			}
			void Init() {
				static bool IfInit = false;
				if (!IfInit) {
					IfInit = true;
					std::atexit(AtExit);
				}
			}
		}
		namespace InputRecorder {
			void StartRecording() {
				impl::Init();
				HWND windowHandle = FindWindow(NULL, _T("FINAL FANTASY XIV"));
				DWORD threadId = GetWindowThreadProcessId(windowHandle, NULL);
				impl::KeyHookWnd = SetWindowsHookExA(WH_KEYBOARD, impl::Hookproc, NULL, threadId);
				if (!impl::KeyHookWnd) {
					DWORD errCode = GetLastError();
					std::string errCmd = "Failed to Hook";
					errCmd += "\nErr-Code: ";
					errCmd += std::to_string((long long)errCode);
					MessageBoxA(NULL, errCmd.c_str(), "Failed to Hook", NULL);
				}
			}
			void StopRecording() {
				if (impl::KeyHookWnd) UnhookWindowsHookEx(impl::KeyHookWnd);
			}
			void PlayBackRecorded() {
				StopRecording();
				impl::inputs.playback();
			}
		}
	}
}