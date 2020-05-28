#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <list>
#include <functional>
#include <memory>

#include "geometry.h"
#include "text.h"
#include "eventhandler.h"


struct SavableData 
{
	Character background{};
	Character textbox{};
	Field textfield{};
	std::vector<Character> characters{};
};

struct Scene : SavableData
{
	std::string dialog{};
	std::vector<std::vector<std::function<void()>>> command_history;
	std::vector<SavableData> saves{};
	std::vector<std::string> script{ {} };
	Character continue_arrow{};

	Scene() = default;
	Scene(const std::string &scene_path);

	void load(const std::string &script_path);
	Character& get_character(const std::string &name);
	Character& get_character(const char* c_str_name) {
		return get_character(std::string(c_str_name));
	}
	void draw();
	void save(int i);
	void restore(int i);
};

#endif 
