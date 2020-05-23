#include <vector>
#include <string>

#ifndef SAVESTATE_H
#define SAVESTATE_H

#include "geometry.h"

struct SceneSaveState 
{
    std::vector<Character> characters;
    Box background;
    Box continue_arrow;
    std::string dialog;
    bool waiting_for_input; 
};

struct SceneStateStack 
{
    std::vector<SceneSaveState> states;
    void update_state();
    void update_state(int i);
    void revert_state(size_t line_num);
};

#endif


#ifndef SCENE_H
#define SCENE_H

#include "geometry.h"

struct Scene
{
	Box background;
	Box textbox;
	Box continue_arrow;

	std::vector<std::string> script;
	std::vector<Character> characters;
	std::string dialog;

	Scene() = default;
	Scene(const std::string &scene_path);

	void load(const std::string &script_path);
	void get_input();
	Character& get_character(const std::string &name);
	Character& get_character(const char* c_str_name) {
		return get_character(std::string(c_str_name));
	}
	void draw();
};

#endif 
