#pragma once
#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")
#include <cstdlib>
#include <memory>
#include <chrono>
#include <thread>

namespace tl {
	namespace types {
		namespace enums {
			enum class Buttons {
				Left, Right, Middle, ScrollButton
			};
			enum class ButtonState {
				down, up
			};
			enum class ScrollState {
				down, up, none
			};
			enum class Keys {
				n1,n2,n3,n4,n5,n6,n7,n8,n9,n0,
				q,w,e,r,t,y,u,i,o,p,
				a,s,d,f,g,h,j,k,l,
				z,x,c,v,b,n,m
			};
			enum class KeyState {
				down, up
			};
		}
		struct Point {
			int x;
			int y;
		};
	}
	namespace userInput {
		class Key {
			tl::types::enums::Keys key;
			tl::types::enums::KeyState state;
			bool upNextTick = false;
		};
		class Keyboard {
		public:
			void WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

			}
		};
		class Click {
			tl::types::Point clickPoint = { 0,0 };
			tl::types::enums::Buttons button;
			tl::types::enums::ButtonState state;
			bool upNextTick = false;
		};
		class Scroll {
			tl::types::Point scrollPoint = { 0,0 };
			tl::types::enums::ScrollState state;
		};
		class Mouse {
		private:
			Click clicks[4];
		public:
			Mouse() {
				using tl::types::enums::Buttons;
				using tl::types::enums::ButtonState;
				updateClick({ 0,0 }, Buttons::Left, ButtonState::up);
				updateClick({ 0,0 }, Buttons::Middle, ButtonState::up);
				updateClick({ 0,0 }, Buttons::Right, ButtonState::up);
				updateClick({ 0,0 }, Buttons::ScrollButton, ButtonState::up);
			}
			void updateClick(tl::types::Point clickPoint, tl::types::enums::Buttons button, tl::types::enums::ButtonState state) {
				switch (button)
				{
				case tl::types::enums::Buttons::Left:
					
					break;
				case tl::types::enums::Buttons::Right:
					break;
				case tl::types::enums::Buttons::Middle:
					break;
				case tl::types::enums::Buttons::ScrollButton:
					break;
				default:
					break;
				}
			}
			void WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
				switch (uMsg)
				{
				case WM_LBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_MBUTTONUP:
				case WM_RBUTTONUP:
				case WM_MOUSEWHEEL:
				case WM_MOUSEMOVE:
					
					return;
				}
			}
		};
	}
	namespace direct2d {
		inline bool ifInit = false;
		inline ID2D1Factory* factory = NULL;
		inline ID2D1HwndRenderTarget* renderTarget = NULL;
		inline ID2D1SolidColorBrush* brush = NULL;
		inline IDWriteFactory* writeFactory = NULL;
		inline IDWriteTextFormat* textFormat = NULL;
		namespace impl {
			D2D1_SIZE_U newWindowSize;
			bool change = false;

			void atExit() {
				if (factory) factory->Release();
				if (renderTarget) renderTarget->Release();
				if (brush) brush->Release();
				if (writeFactory) writeFactory->Release();
				if (textFormat) textFormat->Release();
			}

			bool createFactory() {
				HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
				return (res == S_OK);
			}

			bool createRenderTarget(HWND& windowHandle) {
				RECT rect;
				GetClientRect(windowHandle, &rect);
				HRESULT res = factory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(
						windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
					&renderTarget);
				newWindowSize.width = rect.right;
				newWindowSize.height = rect.bottom;
				return (res == S_OK);
			}

			bool createBrush() {
				HRESULT res = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
				if (res != S_OK) return false;
				return true;
			}

			bool createWriteFactory() {
				HRESULT res = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
					__uuidof(IDWriteFactory),
					reinterpret_cast<IUnknown**>(&writeFactory)
				);
				return (res == S_OK);
			}

			bool createTextFormat() {
				HRESULT res = writeFactory->CreateTextFormat(
					L"Consolas",
					NULL,
					DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STYLE_NORMAL,
					DWRITE_FONT_STRETCH_NORMAL,
					11.0f,
					L"en-us",
					&textFormat
				);
				if (res != S_OK) return false;
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				return (res == S_OK);
			}
		}

		bool Init(HWND windowHandle) {
			std::atexit(impl::atExit);
			if (!impl::createFactory()) return false;
			if (!impl::createRenderTarget(windowHandle)) return false;
			if (!impl::createBrush()) return false;
			if (!impl::createWriteFactory()) return false;
			if (!impl::createTextFormat()) return false;

			tl::direct2d::ifInit = true;
			return true;
		}
		void ResizeRenderTarget(HWND windowHandle) {
			if (!tl::direct2d::ifInit) return;
			impl::change = true;
			RECT rect;
			GetClientRect(windowHandle, &rect);
			impl::newWindowSize.width = rect.right;
			impl::newWindowSize.height = rect.bottom;
		}
		void ResizeRenderTarget(int width, int height) {
			impl::change = true;
			impl::newWindowSize.width = width;
			impl::newWindowSize.height = height;
		}
		void updateChanges() {
			if (!impl::change) return;
			renderTarget->Resize(impl::newWindowSize);
		}
	}
	namespace windows {
		HWND InitWindow(WNDPROC WindowProc, RECT windowSize, const wchar_t* title) {
			WNDCLASSEX windowclass;
			ZeroMemory(&windowclass, sizeof(WNDCLASSEX));
			windowclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			windowclass.hCursor = LoadCursor(NULL, IDC_ARROW);
			windowclass.cbSize = sizeof(WNDCLASSEX);
			windowclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
			windowclass.hInstance = GetModuleHandle(0);
			windowclass.lpfnWndProc = WindowProc;
			windowclass.lpszClassName = L"MainWindow";
			windowclass.style = CS_HREDRAW | CS_VREDRAW;

			RegisterClassExW(&windowclass);

			RECT rect = windowSize;

			HWND windowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", title, WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, windowclass.hInstance, 0);

			if (!windowHandle) return windowHandle;

			ShowWindow(windowHandle, SW_SHOW);
			return windowHandle;
		}
		HWND InitWindow(WNDPROC WindowProc) {
			return InitWindow(WindowProc, { 0,0,800,800 }, NULL);
		}
		void InitDirect2DWindow(WNDPROC WindowProc, RECT windowSize, const wchar_t* title) {
			HWND windowHandle = InitWindow(WindowProc, windowSize, title);
			if (!windowHandle) std::exit(EXIT_FAILURE);
			if (!tl::direct2d::Init(windowHandle)) std::exit(EXIT_FAILURE);
		}
		void InitDirect2DWindow(WNDPROC WindowProc) {
			InitDirect2DWindow(WindowProc, { 0,0,800,800 }, NULL);
		}
		void HideConsole() {
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}
		void SetTitle(const wchar_t* title) {
			SetWindowTextW(GetActiveWindow(), title);
		}
	}
	namespace utility {
		class ConsistantSleep {
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			std::chrono::high_resolution_clock::time_point sleepExtra;
			std::chrono::duration<double> sleepedExtra = std::chrono::milliseconds(0);
			std::chrono::duration<double> toSleep = std::chrono::milliseconds(0);
		public:
			void Start() {
				t1 = std::chrono::high_resolution_clock::now();
			}
			void End(int msTotal) {
				using namespace std;
				sleepExtra = chrono::high_resolution_clock::now();
				auto sleepTime = chrono::milliseconds(msTotal);
				auto timebetweenStartAndEnd = chrono::duration_cast<chrono::duration<double>>
					(chrono::high_resolution_clock::now() - t1);

				toSleep = sleepTime - sleepedExtra - timebetweenStartAndEnd;
				this_thread::sleep_for(toSleep);

				if (toSleep.count() <= 0) toSleep = chrono::milliseconds(0);

				sleepedExtra = chrono::duration_cast<chrono::duration<double>>
					(chrono::high_resolution_clock::now() - sleepExtra) - toSleep;
			}
		};
		class Timer {
			std::chrono::high_resolution_clock::time_point resetTime = std::chrono::high_resolution_clock::now();
		public:
			bool ifTimeReset(int ms) {
				if (ifTime(ms)) {
					reset();
					return true;
				}
				return false;
			}
			bool ifTime(int ms) {
				auto t1 = std::chrono::high_resolution_clock::now();
				auto total = (t1 - resetTime);
				return (ms <= (total.count() / 1000000));
			}
			void reset() {
				resetTime = std::chrono::high_resolution_clock::now();
			}
		};
		namespace window {
			void GetClientPos(int& x, int& y) {
				POINT p = { NULL };
				HWND hwnd = GetActiveWindow();
				if (ClientToScreen(hwnd, &p)) {
					x = p.x;
					y = p.y;
				}
			}
		}
	}
}