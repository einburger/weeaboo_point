#pragma once
#include <string>
#include <vector>

#include "imgui.h"
#include "geometry.h"
#include "gamestate.h"

extern size_t line_number;
extern size_t target_line;

using Args = std::vector<std::string>;

bool parse();
void set_bg(Args);
void play_song(Args);
void place_character(Args);
void move_character(Args);
void set_emotion(Args);
void fade(Args);
void scene_wait(Args);
void write_line(Args);

