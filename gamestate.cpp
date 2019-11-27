#include "stdafx.h"
#include "gamestate.h"
#include "geometry.h"
#include "scene.h"

CursorState cursor_ = { 0, 0, 0.0, 0.0, 0.0, 0.0 };
GameState game_state_ = { NULL, NULL, NULL, NULL, 1280, 720, 120, 0, 0.0, "Game" };
SaveState save_state_ = { 0, 0, 0, 0, 1080, 720 };

// reference these in globals.h
GameState* game_state = &game_state_;
CursorState* cursor = &cursor_;
SaveState* save_state = &save_state_;

void gamestate_window_size_callback(GLFWwindow* window, int width, int height)
{
	game_state->window_height = height;
	game_state->window_width = width;
}

void gamestate_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void gamestate_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		cursor->clicked = 1;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		cursor->holding_left_mouse = 0;
		cursor->clicked = 0;
	}
}

void gamestate_character_callback(GLFWwindow* window, unsigned int codepoint)
{
}

void gamestate_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!game_state->waiting_for_input)
		return;
	if (key == GLFW_KEY_Z || key == GLFW_KEY_SPACE || key == GLFW_KEY_TAB)
	{
		if (action == GLFW_PRESS)
		{
			game_state->waiting_for_input = 0;
			game_state->text_cursor_pos = 0;
			clear_pool(&event_stack);
		}
	}
}

void gamestate_savestate_update(int x, int y, int w, int h, int window_w, int window_h)
{
}

void gamestate_savestate_save(SaveState* save_state)
{
}


void gamestate_savestate_load(SaveState* save_state)
{
}