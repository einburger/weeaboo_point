#pragma once

#include "memorypool.h"

typedef unsigned char uchar;

typedef struct Event Event;

typedef struct Sprite {
	unsigned int texture;
	int w, h;
} Sprite;

typedef struct Box {
	char name[128];
	char clicked;
	int x_min, y_min
		, x_max, y_max
		, w, h
		, x_pos_when_clicked
		, y_pos_when_clicked
		, target_pos;
	float r, g, b, a, wait_time, speed;
	Sprite sprite;
} Box, Character;



// box
Box* character_create(MemoryPool* pool, int x, int y, int w, int h);
Box* geometry_box_create(int x, int y, int w, int h);
void geometry_box_ID(Box*, const char* ID);
void geometry_box_texture(Box*, const char *file_name);
void geometry_box_position(Box*, int x, int y);
void geometry_box_color(Box*, float r, float g, float b, float a);
void geometry_box_size(Box*, int x, int y);
void geometry_box_scale(Box*);
void geometry_box_draw(Box*);

// line
void geometry_line_draw(int x0, int y0, int x1, int y1);

char geometry_box_move(Box* box, int x, int y, int speed);
char geometry_box_wait(double seconds);
char geometry_box_fade(Box* box, float speed);

// collisions
static char geometry_box_check_collision(Box*);
void		geometry_box_on_click(Box*);
