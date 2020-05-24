#include <filesystem>
#include <algorithm>

#include "fileloader.h"
#include "globals.h"
#include "geometry.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fileloader {

static std::unordered_map<std::string, std::vector<std::string> > sprite_paths;

const std::vector<std::string>& get_paths(const std::string &name) { return sprite_paths[name]; }

// build a vector of paths to sprites of a specific folder / asset
void sniff_files() {
    auto &scene = game_state->scene;
    for (const auto &file : fs::directory_iterator(CHARACTER_PATH)) {
            if (fs::is_directory(file.status())) {
                    const auto character_name = file.path().filename().string();

                    std::vector<std::string> paths;
                    for (const auto &sub_file : fs::directory_iterator(file.path())) {
                        paths.push_back(sub_file.path().string());
                    }
                    sprite_paths[character_name] = paths;
            }
    }

    std::vector<std::string> paths;
    for (const auto &file : fs::directory_iterator(BG_PATH)) {
            paths.push_back(file.path().string());
    }
    sprite_paths["bg"] = paths;
}

Sprite get_sprite(const std::string& fullpath) {
    static std::unordered_map<std::string, Sprite> sprite_cache;

    if (auto it = sprite_cache.find(fullpath); it != sprite_cache.end()) 
    { return it->second; }

    // have to read it in from the harddisk
    return [&](){
        Sprite spr{};

        uchar* img = stbi_load(fullpath.c_str(), &spr.w_h[0], &spr.w_h[1], 0, 4); 
        glGenTextures(1, &spr.texture);
        glBindTexture(GL_TEXTURE_2D, spr.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr.w_h[0], spr.w_h[1], 
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                                 img);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(img);

        sprite_cache[fullpath] = spr;

        return spr;
    }();
}
    /*
    auto &scene = game_state->scene;
    for (const auto &file : fs::directory_iterator(CHARACTER_PATH)) {
            if (fs::is_directory(file.status())) {
                    const auto character_name = file.path().filename().string();
                    scene.characters.push_back(Character(0,0,0,0));
                    auto &character = scene.characters.back();
                    character.rgba[3] = 0.0f;
                    character.name = character_name;

                    character.map.reserve(200);
                    for (const auto &sub_file : fs::directory_iterator(file.path())) {
                            render(sub_file.path().string());

                            auto character_emotion = sub_file.path().filename().stem().string();
                            auto sprite = load_texture(sub_file.path().string());
                            character.map[character_emotion] = sprite;
                            character.high_index = std::max(character.high_index, sprite.texture);
                            character.low_index  = std::min(character.low_index, sprite.texture);
                    }
            }
    }

    scene.background.map.reserve(200); 
    for (const auto &file : fs::directory_iterator(BG_PATH)) {
            render(file.path().string());
            auto background_name  = file.path().filename().stem().string();
            auto sprite = load_texture(file.path().string());
            scene.background.map[background_name] = sprite;
            scene.background.high_index = std::max(scene.background.high_index, sprite.texture);
            scene.background.low_index  = std::min(scene.background.low_index, sprite.texture);
    }
}

*/
} // end namespace

