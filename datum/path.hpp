#pragma once

#include <string>

namespace GYDM {
	__lambda__ int path_next_slash_position(const std::string& path, int start = 0, int fallback = -1);

	__lambda__ std::string path_only(const std::string& path);
	__lambda__ std::string file_name_from_path(const std::string& path);
	__lambda__ std::string file_basename_from_path(const std::string& path);
	__lambda__ std::string file_extension_from_path(const std::string& path);

	__lambda__ std::string directory_path(const char* path);
	__lambda__ std::string directory_path(const std::string& path);

	__lambda__ void enter_digimon_zone(const char* process_path);
	__lambda__ std::string digimon_zonedir();
	__lambda__ std::string digimon_subdir(const char* dirpath);
	__lambda__ std::string digimon_path(const char* file, const char* ext = "", const char* sub_rootdir = "stone");

	__lambda__ void digimon_appdata_setup(const char* appdata_dir);
	__lambda__ std::string digimon_appdata_rootdir();
	__lambda__ std::string digimon_appdata_subdir(const char* dirpath);
	__lambda__ std::string digimon_appdata_path(const char* file, const char* ext = ".png", const char* sub_rootdir = "stone");

	__lambda__ void digimon_mascot_setup(const char* shared_dir);
	__lambda__ std::string digimon_mascot_rootdir();
	__lambda__ std::string digimon_mascot_subdir(const char* dirpath);
	__lambda__ std::string digimon_mascot_path(const char* file, const char* ext = ".png", const char* sub_rootdir = "");
}
