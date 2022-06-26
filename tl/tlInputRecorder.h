#pragma once
#include <Windows.h>
#include <cstdlib>
#include <queue>
#include <iostream>

namespace tl {
	namespace ir {
		struct KeyInput {
			DWORD vkCode = 0;
			DWORD scanCode = 0;
			DWORD flags = 0;
			DWORD time = 0;
			KeyInput(KBDLLHOOKSTRUCT* data) {
				vkCode = data->vkCode;
				scanCode = data->scanCode;
				flags = data->flags;
				time = data->time;
			}
			KeyInput() {};
		};
		namespace impl {
			static HHOOK KeyHookWnd;
			
			
			std::queue<KeyInput> keys;
			void ifNotSamePush(KBDLLHOOKSTRUCT* data) {
				static KeyInput last;
				if (keys.size() == 0) {
					keys.push(data);
					last = keys.back();
					return;
				}
				if (last.scanCode == data->scanCode && last.vkCode == data->vkCode && last.flags == data->flags) return;
				keys.push(data);
				last = keys.back();
			}
			LRESULT Hookproc(int code, WPARAM wParam, LPARAM lParam) {
				if (code < 0) return CallNextHookEx(NULL, code, wParam, lParam);
				ifNotSamePush((KBDLLHOOKSTRUCT*)lParam);
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
			bool recording = false;
		}
		namespace InputRecorder {
			
			void StopRecording();
			void StartRecording() {
				StopRecording();
				impl::Init();
				//HWND windowHandle = FindWindow(NULL, _T("FINAL FANTASY XIV"));
				//DWORD threadId = GetWindowThreadProcessId(windowHandle, NULL);
				impl::KeyHookWnd = SetWindowsHookExA(WH_KEYBOARD_LL, impl::Hookproc, GetModuleHandleA(NULL), NULL);
				if (!impl::KeyHookWnd) {
					DWORD errCode = GetLastError();
					std::string errCmd = "Failed to Hook";
					errCmd += "\nErr-Code: ";
					errCmd += std::to_string((long long)errCode);
					MessageBoxA(NULL, errCmd.c_str(), "Failed to Hook", NULL);
					return;
				}
				impl::recording = true;
			}
			void StopRecording() {
				if (impl::KeyHookWnd) UnhookWindowsHookEx(impl::KeyHookWnd);
				impl::recording = false;
			}
			std::vector<KeyInput> GetRecording() {
				StopRecording();
				std::vector<KeyInput> inputs;
				auto& keys = impl::keys;
				while (!keys.empty()) {
					inputs.push_back(keys.front());
					keys.pop();
				}
				return inputs;
			}
			bool IfRecording() {
				return impl::recording;
			}
		}
	}
}