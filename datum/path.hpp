#pragma once

namespace WarGrey::STEM {
	int path_next_slash_position(const std::string& path, int start = 0, int fallback = -1);

	std::string path_only(std::string& path);
	std::string file_name_from_path(std::string& path);
	std::string file_basename_from_path(std::string& path);
	std::string file_extension_from_path(std::string& path);

	std::string digimon_path(const char* file, const char* ext = "", const char* rootdir = "stone");
}
