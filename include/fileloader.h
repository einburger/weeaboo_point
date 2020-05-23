#ifndef FILELOADER_H
#define FILELOADER_H

namespace fileloader {

namespace fs = std::filesystem;

const fs::path BG_PATH = "../backgrounds/";
const fs::path CHARACTER_PATH = "../characters/";

void load_images();

}

#endif
