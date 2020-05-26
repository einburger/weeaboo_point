#pragma once
#include <cstdint> // size_t
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include <optional>

#include "imgui.h"
#include "geometry.h"
#include "gamestate.h"
#include <iostream>

extern size_t line_number;
extern size_t target_line;

using Args = std::vector<std::string>&;

bool parse();
void set_bg(Args);
void play_song(Args);
void place_character(Args);

void exit_character(Args);
void speed_exit_character(Args);

void move_character(Args);
void speed_move_character(Args);

void set_emotion(Args);

void fade_in(Args);
void speed_fade_in(Args);

void fade_out(Args);
void speed_fade_out(Args);

void scene_wait(Args);

void write_line(Args);
void speed_write_line(Args);



struct ParseEvent
{
	bool do_action() { return parse(); }
};

std::optional<std::function<void(int)>>
timed_call(double time, std::function<void(int)> f);

struct MoveGraph  
{
	Character* ch;
	ImVector <ImVec2> poses;
	int step = 0;

	MoveGraph() = default;
	MoveGraph(std::tuple<Character*, ImVector<ImVec2> > &args) {
		ch = std::get<0>(args);
		poses = std::get<1>(args);
	}

	bool do_action()
	{
		if (auto f = timed_call(0.001, [&](int val){ ch->set_x(((300.0f - val)/300.0f) * GameState::w_h[0]); } ))
		{
			(*f)(poses[step++].y);
		}
		return step < poses.size();
	}
};
