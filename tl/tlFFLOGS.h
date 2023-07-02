#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace tl {
	namespace fflogs {
		struct Event {
			std::string time;
			std::string name;
		};
		struct RenameEvent {
			std::string oldName;
			std::string newName;
		};
		struct RenameEvents {
			std::vector<RenameEvent> renamingNames;
		};
		struct Events {
			std::vector<Event> list;

			void fromPrepareDmgStringVector(std::vector<std::string> preparedDmgLines) {
				for (const std::string& line : preparedDmgLines) {
					std::string::size_type timeStart = line.find('"');
					std::string::size_type timeEnd = line.find('"', timeStart + 1);
					std::string::size_type nameStart = line.find('"', timeEnd + 1);
					std::string::size_type nameEnd = line.find('"', nameStart + 1);
					std::string time = line.substr(timeStart + 1, timeEnd - timeStart - 1);
					std::string name = line.substr(nameStart + 1, nameEnd - nameStart - 1);
					this->list.push_back({ time, name });
				}
			}
			void filterDuplicates() {
				std::vector<Event> nonDuplicates;
				nonDuplicates.push_back(list.front());
				struct TimeEvent {
					int milliseconds = 0;
					void FromString(std::string s) {
						std::string::size_type start2 = s.find(':');
						std::string::size_type start3 = s.find('.');
						int minutes = std::stoi(s.substr(0, 2));
						int seconds = std::stoi(s.substr(start2 + 1, 2));
						int tempmilliseconds = std::stoi(s.substr(start3 + 1, 3));
						this->milliseconds = minutes * 60 * 1000 + seconds * 1000 + tempmilliseconds;
					}
				};
				for (const Event& e : list) {
					TimeEvent oldTime;
					TimeEvent newTime;
					oldTime.FromString(nonDuplicates.back().time);
					newTime.FromString(e.time);
					if (oldTime.milliseconds == newTime.milliseconds) continue;
					if (newTime.milliseconds <= oldTime.milliseconds + 2) continue;
					nonDuplicates.push_back(e);
				}
				this->list = nonDuplicates;
			}
			void combineAutoAttacks() {
				std::vector<Event> events;
				int count = 0;
				std::string firstAutoTime;
				for (const Event& e : list) {
					if (e.name.find(" prepares  attack ") != std::string::npos) {
						if (count == 0) firstAutoTime = e.time;
						count++;
						continue;
					}
					else {
						if (count > 0) {
							std::string autoCountString = std::to_string(count) + "x Autos";
							events.push_back({ firstAutoTime, autoCountString });
						}
						events.push_back(e);
						count = 0;
					}
				}
				this->list = events;
			}
			void removeAutoAttacks() {
				std::vector<Event> events;
				for (const Event& e : list) {
					if (e.name.find(" prepares  attack ") != std::string::npos) continue;
					if (e.name.find("x Autos") != std::string::npos) continue;
					events.push_back(e);
				}
				this->list = events;
			}
			void simplifyEventNames() {
				std::vector<Event> events;
				for (const Event& e : list) {
					std::string::size_type nameStart = e.name.find("  ");
					if (nameStart == std::string::npos) {
						events.push_back(e);
						continue;
					}
					std::string::size_type nameEnd = e.name.find("  ", nameStart + 2);
					std::string name = e.name.substr(nameStart + 2, nameEnd - nameStart - 2);
					events.push_back(e);
					events.back().name = name;
				}
				this->list = events;
			}
			void renameSimplifiedEventNames(const RenameEvents& events) {
				for (Event& e : list) {
					bool changed = false;
					for (const RenameEvent& newName : events.renamingNames) {
						if (e.name == newName.oldName) {
							e.name = newName.newName;
							changed = true;
						}
					}
					if (!changed) {
						if (e.name.find("x Autos") == std::string::npos) {
							e.name += " --";
						}
					}
				}
			}
		};

		std::vector<std::string> getDmgTakenLines(std::filesystem::path file) {
			std::vector<std::string> prepareDmgLines;
			std::fstream fs(file);
			for (std::string line; std::getline(fs, line);) {
				if (line.find("prepares") == std::string::npos) continue;
				prepareDmgLines.push_back(line);
			}
			return prepareDmgLines;
		}

		Events getEventsFromFile(std::filesystem::path file) {
			std::vector<std::string> allPreparedDmgLines = getDmgTakenLines(file);
			Events events;
			events.fromPrepareDmgStringVector(allPreparedDmgLines);
			return events;
		}
	}
}