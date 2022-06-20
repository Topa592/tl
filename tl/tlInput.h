#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <queue>
#include <iostream>

namespace tl {
	namespace i {
		void SendKey(char c) {
            keybd_event(c, c, NULL, NULL);
            Sleep(200);
            keybd_event(c, c, KEYEVENTF_KEYUP, NULL);
		}
        void SendModifiedKey(char c, char modifier) {
            keybd_event(modifier, modifier, NULL, NULL);
            SendKey(c);
            keybd_event(modifier, modifier, KEYEVENTF_KEYUP, NULL);
        }
	}
}