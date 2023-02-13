#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include <vector>

namespace tl {
	//imgui
	namespace ig {
		namespace StaticWindowDrawer {
			namespace impl {
				std::vector<void (*)()> windowDrawFunctions;
				void DrawAll() {
					for (int i = 0; i < windowDrawFunctions.size(); i++) {
						windowDrawFunctions[i]();
					}
				}
			}

			void AddWindow(void (*func)()) {
				impl::windowDrawFunctions.push_back(func);
			}
			void DrawAll() {
				impl::DrawAll();
			}
		}		
	}
}