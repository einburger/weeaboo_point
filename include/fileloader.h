#ifndef FILELOADER_H
#define FILELOADER_H

#include <filesystem>
#include "geometry.h"

namespace fileloader {

	namespace fs = std::filesystem;

	const fs::path BG_PATH = "../backgrounds/";
	const fs::path CHARACTER_PATH = "../characters/";

	void sniff_files();
	const std::vector<std::string> & get_paths(const std::string &name); 
	Sprite load_sprite(const std::string& fullpath);

}

#endif
