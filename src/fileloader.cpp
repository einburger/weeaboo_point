#include <filesystem>
#include <algorithm>

#include "fileloader.h"
#include "globals.h"
#include "geometry.h"

namespace fileloader {

void load_images() {

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
                            auto character_emotion = sub_file.path().filename().stem().string();
                            auto sprite = character.load_texture(sub_file.path().string());
                            character.map[character_emotion] = sprite;
                            character.high_index = std::max(character.high_index, sprite.texture);
                            character.low_index  = std::min(character.low_index, sprite.texture);
                    }
            }
    }

    scene.background.map.reserve(200); 
    for (const auto &file : fs::directory_iterator(BG_PATH)) {
            auto background_name  = file.path().filename().stem().string();
            auto sprite = scene.background.load_texture(file.path().string());
            scene.background.map[background_name] = sprite;
            scene.background.high_index = std::max(scene.background.high_index, sprite.texture);
            scene.background.low_index  = std::min(scene.background.low_index, sprite.texture);
    }
}

}

