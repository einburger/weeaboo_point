#include <cmath> // for sin
#include <fstream>
#include <filesystem>
#include <iostream>

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
	textbox.set_size(GameState::w_h[0] * 0.6, GameState::w_h[1] * 0.2);
	textbox.set_pos(GameState::w_h[0] * 0.2, GameState::w_h[1] * 0.7);
	//textbox.set_alpha(0.7f);
	textbox.set_color(std::array<float,4>{ 0,0,0,0.7 });
	//textbox.set_texture(std::string(TEXTBOX_BG_PATH + "ptext.png"));
	//textbox.set_to_sprite_size();

	textfield = Field("arialbd.ttf", 32.0, textbox.min_xy[0], textbox.min_xy[1]);

	// create and initialize bg
	background = Character(0, 0, GameState::w_h[0], GameState::w_h[1]);
	background.set_alpha(1.0f);
	background.sprite_paths = fileloader::get_paths("bg");

	continue_arrow = Character();
	continue_arrow.set_texture(std::string(TEXTBOX_BG_PATH + "Untitled1.png"));
	continue_arrow.set_alpha(1.0f);
	const int w = continue_arrow.sprite.w_h[0] * 0.2;
	const int h = continue_arrow.sprite.w_h[1] * 0.2;
	continue_arrow.set_to_sprite_size();
	continue_arrow.set_size(w, h);
	continue_arrow.set_pos(textbox.max_xy[0]-(w+2), textbox.max_xy[1]-(h+2));

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
}

void Scene::draw()
{
	background.draw();

	for (auto& character : characters)
	{
		character.draw();
	}

	textbox.draw();
	if (!GameState::scene.dialog.empty())
		textfield.dl.draw();


	//int x = GameState::w_h[0] * 0.60 + 5 * sin(clock() * 0.009);

	auto& arrow = continue_arrow;
	//arrow.set_pos(x, arrow.min_xy[1]);
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
	characters.back().sprite_paths = fileloader::get_paths(name);
	return characters.back();
}
