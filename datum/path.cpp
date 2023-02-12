#include "string.hpp"
#include "path.hpp"

#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
static std::string zonedir;

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
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_slash_idx = last_slash_position(raw, size);

	if (last_slash_idx >= 0) {
		dirname = substring(path, 0, last_slash_idx + 1);
	}

	return dirname;
}

std::string WarGrey::STEM::file_name_from_path(const std::string& path) {
	std::string filename = path;
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_slash_idx = last_slash_position(raw, size);
	
	if (last_slash_idx >= 0) { // TODO: how to deal with directories?
		filename = substring(path, last_slash_idx + 1, size);
	}

	return filename;
}

std::string WarGrey::STEM::file_basename_from_path(const std::string& path) {
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_dot_idx = last_dot_position(raw, size, size);
	int last_slash_idx = last_slash_position(raw, last_dot_idx, -1);
	
	return substring(path, last_slash_idx + 1, last_dot_idx);
}

std::string WarGrey::STEM::file_extension_from_path(const std::string& path) {
	std::string ext;
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_dot_idx = last_dot_position(raw, size);
	
	if (last_dot_idx >= 0) {
		ext = substring(path, last_dot_idx, size);
	}

	return ext;
}

std::string WarGrey::STEM::directory_path(const char* path) {
	return directory_path(std::string(path));
}

std::string WarGrey::STEM::directory_path(const std::string& path) {
	char sep = path::preferred_separator;
	std::string npath = path;
	
	if (npath.empty()) {
		npath = sep;
	} else if (path.back() != sep) {
	 	npath.push_back(sep);
	}

	return npath;
}

void WarGrey::STEM::enter_digimon_zone(const char* process_path) {
	static std::string info_rkt = std::string(1, path::preferred_separator).append("info.rkt");
	static path rootdir = current_path().root_directory();
	path ppath = (process_path == nullptr) ? current_path().append(info_rkt) : canonical(current_path().append(process_path));
	
	zonedir.clear();
	while (zonedir.empty() && (ppath != rootdir)) {
		ppath = ppath.parent_path();
	
		if (exists(path(ppath.string().append(info_rkt)))) {
			zonedir = ppath.string();
		}
	}

	if (zonedir.empty()) {
		zonedir = current_path().string();
	}
	
	zonedir.push_back(path::preferred_separator);
}

std::string WarGrey::STEM::digimon_zonedir() {
	if (zonedir.empty()) {
		enter_digimon_zone(nullptr);
	}

	return zonedir;
}

std::string WarGrey::STEM::digimon_path(const char* file, const char* ext, const char* rootdir) {
	std::string file_raw(file);
	std::string root_dir(rootdir);
	std::string file_ext = (file_extension_from_path(file_raw) == "") ? (file_raw.append(ext)) : file_raw;
	std::string path_ext = ((root_dir == "") ? file_ext : (directory_path(root_dir).append(file_ext)));

    return zonedir.append(path_ext);
}
