#pragma once

#include <cstdio> // for FILE
#include <GLFW/glfw3.h> // GLFWwindow

#include "geometry.h"
#include "scene.h"

namespace GameState {
	extern GLFWwindow*			current_window;
	extern Scene				scene;
	extern std::array<int, 2>	w_h;
	extern int			        fps;
	extern double		        dt;
	extern const char*			window_title;
	extern bool					waiting_for_input;
	extern int					text_cursor_pos;
}

// callbacks
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void gamestate_character_callback(GLFWwindow* window, unsigned int codepoint);
void gamestate_key_callback(GLFWwindow* window, int key, int seconds, int action, int mods);
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_framebuffer_size_callback(GLFWwindow* window, int width, int height);
