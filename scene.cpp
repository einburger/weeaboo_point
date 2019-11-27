#include "stdafx.h"
#include "scene.h"
#include "globals.h"
#include "geometry.h"
#include "memorypool.h"
#include "text.h"
#include "parser.h"


Scene* scene_create(const char* scene_script)
{
	Scene* scene = PUSH_OBJECT(&memory_pool, Scene);

	// create and initialize textbox
	scene->textbox = geometry_box_create(game_state->window_width * 0.14, 
										 game_state->window_height * 0.28, 
										 game_state->window_width * 0.5, 
										 game_state->window_height * 0.1);
	scene->textbox->a = 0.7;
	geometry_box_ID(scene->textbox, "textbox");
	geometry_box_color(scene->textbox, 1.0, 1.0, 1.0, 0.7);

	char path[MAX_PATH];
	text_append(path, TEXTBOX_BG_PATH, "ptext.png");
	geometry_box_texture(scene->textbox, path);
	geometry_box_size(scene->textbox, 
					  scene->textbox->sprite.w, 
					  scene->textbox->sprite.h);

	// create and initialize bg
	scene->background = geometry_box_create(0, 0, game_state->window_width, game_state->window_height);
	geometry_box_ID(scene->background, "bg");

	// set up memory pool for the script
	script_pool.base = memory_pool.base + memory_pool.used_size;
	script_pool.total_size = sizeof(char) * 10'000'000;
	script_pool.used_size = 0;

	// set up location for character_pool
	character_pool.base = script_pool.base + script_pool.used_size;
	character_pool.total_size = sizeof(Character) * scene_file_count();
	character_pool.used_size = 0;

	// manages events
	event_stack.base = character_pool.base + character_pool.total_size;
	event_stack.total_size = sizeof(Event) * 20;
	event_stack.used_size = 0;

	// load script
	strcpy(scene->dialog, "");
	scene_load(scene_script);
	text_init("arialbd.ttf", 32.0);

	return scene;
}

void scene_load(const char* scene_script)
{
	if (game_state->current_file)
		fclose(game_state->current_file);

	game_state->current_file = fopen(scene_script, "r");
	read_file();
	fclose(game_state->current_file);
}

void scene_draw()
{
	if (!game_state->scene->background || !game_state->scene->textbox) { ERROR_REPORT; }

	geometry_box_draw(game_state->scene->background);

	for (size_t i = 0; i * sizeof(Character) < character_pool.used_size; ++i)
	{
		Character* current = ((Character*)(character_pool.base)) + i;
		geometry_box_draw(current);
	}

	geometry_box_draw(game_state->scene->textbox);
	text_draw(game_state->scene->dialog);
}

size_t scene_file_count()
{
	WIN32_FIND_DATA find_file_data;
	HANDLE handle;
	TCHAR szDir[MAX_PATH];

	TCHAR folder_path[] = TEXT("characters\\*");
	StringCchCopy(szDir, MAX_PATH, folder_path);

	size_t count = 0;
	handle = FindFirstFile(szDir, &find_file_data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(handle, &find_file_data) != 0)
		{
			count++;
		}
	}
	FindClose(handle);
	return count - 1;
}

void scene_get_input()
{
	game_state->waiting_for_input = 1;
}

Character* scene_get_character(const char* name)
{
	for (size_t i = 0; i * sizeof(Character) < character_pool.used_size; ++i)
	{
		Character* current = ((Character*)(character_pool.base)) + i;
		if (strcmp(name, current->name) == 0)
		{
			return current;
		}
	}

	Character* character = character_create(&character_pool, 0, 0, 0, 0);
	strcpy(character->name, name);

	static char path[1024];
	text_append(path, CHARACTER_PATH, name);
	strcat(strcat(strcat(path, "/"), name), ".png");
	geometry_box_texture(character, path);

	character->a = 0.0;

	return character;
}