#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "tlAHK.h"

namespace tl {
	namespace ffxiv {
		class CraftMacro {
		private:
			const std::vector<int> sleepTimes;
			std::vector<int> GetMacroSleepTime(const std::string& file) {
				std::ifstream fs(file);
				std::vector<int> sleepTimes = { 0 };
				for (std::string line; std::getline(fs, line);) {
					if (line == "") {
						sleepTimes.push_back(0);
						continue;
					}
					std::string::size_type loc = line.find_last_of("<wait.");
					std::string::size_type locEnd = line.find('>', loc);
					if (loc == std::string::npos || locEnd == std::string::npos) continue;
					loc++;
					std::string::size_type length = locEnd - loc;
					std::string number = line.substr(loc, length);
					int num = std::stoi(number);
					sleepTimes.back() += num;
				}
				return sleepTimes;
			}
		public:
			int MacroCount() {
				return sleepTimes.size();
			}
			const std::vector<int>& GetSleepTimes() {
				return sleepTimes;
			}
			CraftMacro(const std::string& file)
				: sleepTimes(GetMacroSleepTime(file))
			{

			}
		};

		class CraftingScript : public tl::ahk::SingleScriptRuntime {
			tl::ffxiv::CraftMacro macro;
		public:
			CraftingScript(
				const std::string& title,
				const std::string& scriptDirectPath,
				const std::string& macroDirectPath
			) : tl::ahk::SingleScriptRuntime{ title, scriptDirectPath }
				, macro(macroDirectPath) 
			{

			}
		};
	}
}