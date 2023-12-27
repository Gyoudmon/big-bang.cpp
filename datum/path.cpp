#include "string.hpp"
#include "path.hpp"

#include <filesystem>

using namespace GYDM;
using namespace std::filesystem;

/*************************************************************************************************/
static std::string zonedir;
static std::string appdatadir;
static std::string mascotdir;

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
int GYDM::path_next_slash_position(const std::string& path, int start, int fallback) {
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

std::string GYDM::path_only(const std::string& path) {
	std::string dirname;
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_slash_idx = last_slash_position(raw, size);

	if (last_slash_idx >= 0) {
		dirname = substring(path, 0, last_slash_idx + 1);
	}

	return dirname;
}

std::string GYDM::file_name_from_path(const std::string& path) {
	std::string filename = path;
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_slash_idx = last_slash_position(raw, size);

	if (last_slash_idx >= 0) { // TODO: how to deal with directories?
		filename = substring(path, last_slash_idx + 1, size);
	}

	return filename;
}

std::string GYDM::file_basename_from_path(const std::string& path) {
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_dot_idx = last_dot_position(raw, size, size);
	int last_slash_idx = last_slash_position(raw, last_dot_idx, -1);
	
	return substring(path, last_slash_idx + 1, last_dot_idx);
}

std::string GYDM::file_extension_from_path(const std::string& path) {
	std::string ext;
	int size = int(path.size());
	const char* raw = path.c_str();
	int last_dot_idx = last_dot_position(raw, size);
	
	if (last_dot_idx >= 0) {
		ext = substring(path, last_dot_idx, size);
	}

	return ext;
}

std::string GYDM::directory_path(const char* path) {
	return directory_path(std::string(path));
}

std::string GYDM::directory_path(const std::string& path) {
	char sep = path::preferred_separator;
	std::string npath = path;
	
	if (npath.empty()) {
		npath = sep;
	} else if (path.back() != sep) {
	 	npath.push_back(sep);
	}

	return npath;
}

void GYDM::enter_digimon_zone(const char* process_path) {
	static std::string info_rkt = std::string(1, path::preferred_separator).append("info.rkt");
	static path rootdir = current_path().root_path();
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

void GYDM::digimon_appdata_setup(const char* appdata_path) {
	if ((appdata_path != nullptr) && (appdata_path[0] != '\0')) {
		path folder = path(appdata_path).make_preferred();

		if (folder.is_absolute()) {
			appdatadir = directory_path(folder.string());
		} else {
			appdatadir = digimon_subdir(folder.string().c_str());
		}
	}
}

void GYDM::digimon_mascot_setup(const char* shared_path) {
	if ((shared_path != nullptr) && (shared_path[0] != '\0')) {
		path folder = path(shared_path).make_preferred();

		if (folder.is_absolute()) {
			mascotdir = directory_path(folder.string());
		} else {
			mascotdir = digimon_subdir(folder.string().c_str());
		}
	}
}

std::string GYDM::digimon_zonedir() {
	if (zonedir.empty()) {
		enter_digimon_zone(nullptr);
	}

	return directory_path(zonedir);
}

std::string GYDM::digimon_subdir(const char* dirpath) {
	std::string subdir = directory_path(path(dirpath).make_preferred().string());

	return digimon_zonedir().append(subdir);
}

std::string GYDM::digimon_appdata_rootdir() {
	if (appdatadir.empty()) {
		return digimon_subdir("stone");
	} else {
		return directory_path(appdatadir);
	}
}

std::string GYDM::digimon_appdata_subdir(const char* dirpath) {
	std::string subdir = directory_path(path(dirpath).make_preferred().string());

	return digimon_appdata_rootdir().append(subdir);
}

std::string GYDM::digimon_mascot_rootdir() {
	if (mascotdir.empty()) {
		return digimon_subdir("stone/mascot");
	} else {
		return directory_path(mascotdir);
	}
}

std::string GYDM::digimon_mascot_subdir(const char* dirpath) {
	std::string subdir = directory_path(path(dirpath).make_preferred().string());

	return digimon_mascot_rootdir().append(subdir);
}

std::string GYDM::digimon_path(const char* file, const char* ext, const char* sub_rootdir) {
	std::string root_dir(sub_rootdir);
	std::string file_raw = path(file).make_preferred().string();
	std::string file_ext = (file_extension_from_path(file_raw) == "") ? (file_raw.append(ext)) : file_raw;
	std::string path_ext = ((root_dir == "") ? file_ext : (directory_path(path(sub_rootdir).make_preferred().string()).append(file_ext)));

    return directory_path(zonedir).append(path_ext);
}

std::string GYDM::digimon_appdata_path(const char* file, const char* ext, const char* sub_rootdir) {
	std::string root_dir(sub_rootdir);
	std::string file_raw = path(file).make_preferred().string();
	std::string file_ext = (file_extension_from_path(file_raw) == "") ? (file_raw.append(ext)) : file_raw;
	std::string path_ext = ((root_dir == "") ? file_ext : (directory_path(path(sub_rootdir).make_preferred().string()).append(file_ext)));

    return digimon_appdata_rootdir().append(path_ext);
}

std::string GYDM::digimon_mascot_path(const char* file, const char* ext, const char* sub_rootdir) {
	std::string root_dir(sub_rootdir);
	std::string file_raw = path(file).make_preferred().string();
	std::string file_ext = (file_extension_from_path(file_raw) == "") ? (file_raw.append(ext)) : file_raw;
	std::string path_ext = ((root_dir == "") ? file_ext : (directory_path(path(sub_rootdir).make_preferred().string()).append(file_ext)));

    return digimon_mascot_rootdir().append(path_ext);
}
