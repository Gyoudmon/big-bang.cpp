#include "string.hpp"
#include "path.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
static int last_slash_position(const char* raw, int size, int fallback = -1) {
	int index = fallback;

	for (int idx = size - 1; idx >= 0; idx--) {
		if ((raw[idx] == '/') || (raw[idx] == '\\')) {
			index = idx;
			break;
		}
	}

	return index;
}

static int last_dot_position(const char* raw, int size, int fallback = -1) {
	int index = fallback;

	for (int idx = size - 1; idx > 0; idx--) { // NOTE: do not count on the leading "." for dot files
		if (raw[idx] == '.') {
			index = idx;
			break;
		}
	}

	return index;
}

/*************************************************************************************************/
int WarGrey::STEM::path_next_slash_position(const std::string& path, int start, int fallback) {
	const char* raw = path.c_str();
	size_t size = path.size();
	int index = fallback;

	for (int idx = start; idx < size; idx++) {
		if ((raw[idx] == '/') || (raw[idx] == '\\')) {
			index = idx;
			break;
		}
	}

	return index;
}

std::string WarGrey::STEM::path_only(const std::string& path) {
	std::string dirname;
	size_t size = path.size();
	const char* raw = path.c_str();
	int last_slash_idx = last_slash_position(raw, size);

	if (last_slash_idx >= 0) {
		dirname = substring(path, 0, last_slash_idx + 1);
	}

	return dirname;
}

std::string WarGrey::STEM::file_name_from_path(const std::string& path) {
	std::string filename = path;
	size_t size = path.size();
	const char* raw = path.c_str();
	int last_slash_idx = last_slash_position(raw, size);
	
	if (last_slash_idx >= 0) { // TODO: how to deal with directories?
		filename = substring(path, last_slash_idx + 1, size);
	}

	return filename;
}

std::string WarGrey::STEM::file_basename_from_path(const std::string& path) {
	size_t size = path.size();
	const char* raw = path.c_str();
	int last_dot_idx = last_dot_position(raw, size, size);
	int last_slash_idx = last_slash_position(raw, last_dot_idx, -1);
	
	return substring(path, last_slash_idx + 1, last_dot_idx);
}

std::string WarGrey::STEM::file_extension_from_path(const std::string& path) {
	std::string ext;
	size_t size = path.size();
	const char* raw = path.c_str();
	int last_dot_idx = last_dot_position(raw, size);
	
	if (last_dot_idx >= 0) {
		ext = substring(path, last_dot_idx, size);
	}

	return ext;
}

std::string WarGrey::STEM::digimon_path(const char* file, const char* ext, const char* rootdir) {
	std::string file_raw(file);
	std::string root_dir(rootdir);
	std::string file_ext = (file_extension_from_path(file_raw) == "") ? (file_raw.append(ext)) : file_raw;
	std::string path_ext = ((root_dir == "") ? file_ext : (root_dir.append("/").append(file_ext)));

	return std::string(__ZONE__).append(path_ext);
}
