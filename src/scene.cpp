#include <fstream>
#include <filesystem>
#include <iostream>

#include "gamestate.h"
#include "scene.h"
#include "geometry.h" // box
#include "text.h" // text_draw
#include "globals.h" // PATH
#include "fileloader.h"

Scene::Scene(const std::string& scene_script)
{
	fileloader::sniff_files();

	//create and initialize textbox
	textbox = Character();
	textbox.set_size(GameState::w_h.x * 0.6, GameState::w_h.y * 0.2);
	textbox.set_pos(GameState::w_h.x * 0.5, GameState::w_h.y * 0.8);
	textbox.set_color(v4<float>(0,0,0,0.7));

	textfield = Field("arialbd.ttf", 32.0, textbox.min_xy.x, textbox.min_xy.y);

	// create and initialize bg
	background = Character(0, 0, GameState::w_h.x, GameState::w_h.y);
	background.set_alpha(1.0f);
	background.sprite_paths = fileloader::get_paths("bg");

	continue_arrow = Character();
	continue_arrow.set_texture(std::string(TEXTBOX_BG_PATH + "Untitled1.png"));
	continue_arrow.set_alpha(1.0f);
	const int w = continue_arrow.sprite.wdth_hght.x * 0.2;
	const int h = continue_arrow.sprite.wdth_hght.y * 0.2;
	continue_arrow.set_to_sprite_size();
	continue_arrow.set_size(w, h);
	continue_arrow.set_pos(textbox.max_xy.x-(w+2), textbox.max_xy.y-(h+2));

	load(scene_script);
}

void Scene::load(const std::string& scriptpath)
{
	auto file = std::ifstream(scriptpath);
	if (!file.is_open())
	{
		exit(EXIT_FAILURE);
	}

	for (std::string line; std::getline(file, line); /* empty */)
	{
		if (!line.empty()) script.push_back(line);
	}

	file.close();

	command_history.resize(script.size());
}

void Scene::draw()
{
	background.draw();

	for (auto& character : characters) { character.draw(); }

	textbox.draw();
	textfield.dl.draw();

	auto& arrow = continue_arrow;
	if (GameState::waiting_for_input)
		continue_arrow.draw();
}

Character& Scene::get_character(const std::string& name)
{
	for (auto& character : characters)
	{
		if (character.name == name)
		{
			return character;
		}
	}
	characters.push_back(Character(name));
	characters.back().set_pos(0, GameState::w_h.y * 0.8);
	characters.back().sprite_paths = fileloader::get_paths(name);
	return characters.back();
}

void Scene::save(int i)
{
	if (i == saves.size()) {
		saves.push_back(SavableData{});
	}
	saves[i].background = background;
	saves[i].textbox = textbox;
	saves[i].textfield = textfield;
	saves[i].characters = characters;
}

void Scene::restore(int i)
{
	if (i >= saves.size())
	{
		return;
	}

	GameState::parsing = false;
	GameState::waiting_for_input = false;

	event_handler->events.clear();
	GameState::text_cursor_pos = 0;

	textbox = saves[i].textbox;
	background = saves[i].background;
	characters = saves[i].characters;
	GameState::scene.textfield.dl = DialogLines();
	for (int k{}; k < command_history[i].size(); ++k)
		command_history[i][k]();
}
