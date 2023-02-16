#pragma once

namespace WarGrey::STEM {
	int path_next_slash_position(const std::string& path, int start = 0, int fallback = -1);

	std::string path_only(const std::string& path);
	std::string file_name_from_path(const std::string& path);
	std::string file_basename_from_path(const std::string& path);
	std::string file_extension_from_path(const std::string& path);

	std::string directory_path(const char* path);
	std::string directory_path(const std::string& path);

	void enter_digimon_zone(const char* process_path);
	std::string digimon_zonedir();
	std::string digimon_path(const char* file, const char* ext = "", const char* rootdir = "stone");
	std::string digimon_mascot_path(const char* file, const char* ext = ".png", const char* rootdir = "digitama/big_bang/mascot");
}
