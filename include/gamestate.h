#pragma once

#include <cstdio> // for FILE
#include <GLFW/glfw3.h> // GLFWwindow

#include "geometry.h"
#include "scene.h"

namespace GameState {
	extern GLFWwindow*			current_window;
	extern Scene				scene;
	extern v2<int>				w_h;
	extern double			    fps;
	extern double		        dt;
	extern double				curr_time;
	extern double				prev_time;
	extern const char*			window_title;
	extern bool					waiting_for_input;
	extern int					text_cursor_pos;
	extern bool					parsing;
	extern int					restore_point;
}

// callbacks
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void gamestate_character_callback(GLFWwindow* window, unsigned int codepoint);
void gamestate_key_callback(GLFWwindow* window, int key, int seconds, int action, int mods);
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_framebuffer_size_callback(GLFWwindow* window, int width, int height);
