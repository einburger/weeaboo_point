#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>

#include "geometry.h"
#include "text.h"

struct Savable
{
	Character background{};
	Character textbox{};
	std::vector<std::string> script{ {} };
	std::string dialog{};
	std::vector<Character> characters{};

	Field textfield{};

	void save();
};

struct Scene : Savable
{
	Character continue_arrow{};

	Scene() = default;
	Scene(const std::string &scene_path);

	void load(const std::string &script_path);
	Character& get_character(const std::string &name);
	Character& get_character(const char* c_str_name) {
		return get_character(std::string(c_str_name));
	}
	void draw();
};

#endif 
