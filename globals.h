#pragma once

#include "stdafx.h"
#include "gamestate.h"
#include "memorypool.h"

#define BG_PATH "backgrounds/"
#define CHARACTER_PATH "characters/"
#define FONT_PATH "fonts/"
#define SCRIPT_PATH "scripts/"
#define TEXTBOX_BG_PATH "textbox_bg/"

#define ERROR_REPORT \
printf("ERROR:\n    line: %d \n    file: %s\n", __LINE__, __FILE__); \
exit(EXIT_FAILURE);

#define arr_size(a) sizeof(a) / sizeof(&a[0])

extern GameState   *game_state;
extern CursorState *cursor;
extern SaveState   *save_state;

MemoryPool memory_pool;
MemoryPool script_pool;
MemoryPool character_pool; // doesn't actually own memory itself, just maintains the location
MemoryPool event_stack;

enum EVENT_ID {
	MOVE,
	WAIT,
	FADE,
	PARSE,
	WRITE,
	GET_INPUT
};

enum COLOR {
	BLACK,
	WHITE
};
