#pragma once
#include "globals.h"
#include "geometry.h"

#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <strsafe.h>

extern Box* box; // forward declaration

typedef struct Scene {
	Box *background, *textbox;
	char dialog[1024];
} Scene;

Scene	  *scene_create(const char *scene_script);
void	   scene_load(const char *scene_script);
void	   scene_draw();
void	   scene_get_input();
Character *scene_get_character(const char* name);

// windows file search
size_t scene_file_count();