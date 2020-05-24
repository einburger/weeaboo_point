#ifndef FILELOADER_H
#define FILELOADER_H

#include "geometry.h"

namespace fileloader {

namespace fs = std::filesystem;

const fs::path BG_PATH = "../backgrounds/";
const fs::path CHARACTER_PATH = "../characters/";

const std::vector<std::string> & get_paths(const std::string &name); 
void sniff_files();

Sprite get_sprite(const std::string& fullpath);

}

#endif
