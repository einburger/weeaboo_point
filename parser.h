#pragma once

#define MAX_ARGS 6


typedef struct Line
{
	char line[1024];
} Line;

static size_t line_number = 0;

char parse();
void read_file();
void set_bg(int argc, char** argv);
void play_song(int argc, char** argv);
void place_character(int argc, char** argv);
void set_emotion(int argc, char** argv);
void exit_character(int argc, char** argv);
void move_character(int argc, char** argv);
void fade_in(int argc, char** argv);
void scene_wait(int argc, char** argv);
void write_line(const char* line);
