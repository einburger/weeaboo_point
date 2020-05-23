#pragma once
#include <cstdint> // size_t
#include <string>
#include <vector>

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

