#pragma once
#include <sqlite3.h>
#include <string>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <unordered_map>

namespace tl {
	namespace sql {
		static int callback(void* data, int argc, char** argv, char** azColName) {
			int i;
			fprintf(stderr, "%s: ", (const char*)data);

			for (i = 0; i < argc; i++) {
				printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
			}

			printf("\n");
			return 0;
		}
		class SingleDB {
			sqlite3* db;
			const std::string dbName;
			const std::string tlsqltableName = "tlsql";
			void createTableIfNotExist() {
				//execSql(sqlMsg);
			}
			int execSql(const std::string& sqlMsg) {
				char* zErrMsg = 0;
				int rc;
				const char* sql = sqlMsg.c_str();
				const char* data = "Callback function called";

				/* Open database */
				rc = sqlite3_open(dbName.c_str(), &db);

				if (rc) {
					fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
					return 1;
				}
				else {
					fprintf(stderr, "Opened database successfully\n");
				}

				/* Execute SQL statement */
				rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
				if (rc != SQLITE_OK) {
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				else {
					fprintf(stdout, "Operation done successfully\n");
				}
				sqlite3_close(db);
				return 0;
			}
		public:
			SingleDB(std::string dataBaseName) : dbName(dataBaseName) {

			}
			std::string GetString(std::string name) {
				std::string sqlMsg = "SELECT " + name + " FROM " + tlsqltableName;
			}
			int GetInt(std::string name) {

			}
			bool GetBool(std::string name) {

			}
		};
		
		
		void Draw() {
			ImGui::Begin("tlsql");

			ImGui::End();
		}
	}
}