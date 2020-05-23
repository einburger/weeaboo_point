#include <cmath> // for sin
#include <fstream>
#include <filesystem>
#include <iostream>

#include "scene.h"
#include "geometry.h" // box
#include "text.h" // text_draw
#include "globals.h" // PATH

Scene::Scene (const std::string &scene_script)
{
	// create and initialize textbox
	textbox = Box(game_state->window_width * 0.14, 
		      game_state->window_height * 0.28, 
		      game_state->window_width * 0.5, 
		      game_state->window_height * 0.1);
	textbox.rgba = {1.0f, 1.0f, 1.0f, 0.7f};
	textbox.set_texture(std::string(TEXTBOX_BG_PATH + "ptext.png"));
	textbox.set_size(textbox.sprite.w_h);

	// create and initialize bg
	background = Box(0, 0, game_state->window_width, game_state->window_height);
	continue_arrow = Box(game_state->window_width * 0.70, 
							   game_state->window_height * 0.91, 5, 5);
	continue_arrow.set_texture(std::string(TEXTBOX_BG_PATH + "Untitled1.png"));

	int w = continue_arrow.sprite.w_h[0] * 0.2;
	int h = continue_arrow.sprite.w_h[1] * 0.2;
	continue_arrow.set_size({w, h});

	// load script
	load(scene_script);
	text_init("arialbd.ttf", 32.0);
}

void Scene::load(const std::string &path)
{
	auto file = std::ifstream(path);
	if (!file.is_open()) {
		exit(EXIT_FAILURE);
	}

	for (std::string line; std::getline(file, line); /* empty */)
        { if (!line.empty()) script.push_back(line); }

	file.close();

	
	namespace fs = std::filesystem;
	for (const auto &file : fs::directory_iterator(CHARACTER_PATH))
	{
		if (fs::is_directory(file.status())) {
			const auto character_name = file.path().filename().string();

			characters.push_back(Character(0,0,0,0));
			auto &character = characters.back();
			character.rgba[3] = 0.0f;
			character.name = character_name;

			for (const auto &sub_file : fs::directory_iterator(file.path())) {
				auto character_emotion = sub_file.path().filename().stem().string();
				character.emotion_map[character_emotion] = sub_file.path().string();
			}
		}
	}
}

void Scene::draw()
{
	background.draw();

	for (auto& character : characters) {
		character.draw();
	}

	textbox.draw();
	text_draw(dialog.c_str());

	auto &arrow = continue_arrow;

	int x = game_state->window_width * 0.60 + 5 * sin(clock() * 0.009);

	arrow.set_position({x, arrow.min_xy[1]});

	if (game_state->waiting_for_input)
		continue_arrow.draw();
}

void Scene::get_input()
{
	game_state->waiting_for_input = true;
}

Character& Scene::get_character(const std::string &name)
{
	for (auto &character : characters) {
		if (character.name == name) {
			return character;
		}
	}
	return characters.back();
}

// savestate stuff

void SceneStateStack::update_state() {
    auto &scene = game_state->scene;

    SceneSaveState st;
    for (const auto& ch : scene.characters) {
        st.characters.push_back(ch);
    }

    st.background = scene.background;
    st.continue_arrow = scene.continue_arrow;
    st.dialog = scene.dialog;
    st.waiting_for_input = game_state->waiting_for_input; 
    states.push_back(st);
}

void SceneStateStack::revert_state(size_t line_num) {
    auto &scene = game_state->scene;

    if (line_num > line_number || line_num < 1) {
        return;
    }
    auto st = states[line_num];
    for (int i{}; i < scene.characters.size(); ++i) {
       scene.characters[i] = st.characters[i]; 
    }

    scene.background = st.background;
    scene.continue_arrow = st.continue_arrow;
    scene.dialog = st.dialog;
    game_state->waiting_for_input = st.waiting_for_input;
    line_number = line_num;
}

