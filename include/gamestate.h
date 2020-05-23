#pragma once

#include <cstdio> // for FILE
#include <GLFW/glfw3.h> // GLFWwindow

#include "geometry.h"
#include "scene.h"

struct CursorState
{
	bool				holding_left_mouse{false};
	bool				clicked{false};
	double				current_x{}; 
	double				current_y{}; 
	double				click_pos_x{}; 
	double				click_pos_y{};

	CursorState() = default;
};

struct GameState
{
	GLFWwindow*			current_window{NULL};
	Box				last_clicked_box;
	Scene				scene;
	int                             window_width{1280};
	int                             window_height{720};
	int			        fps{333};
	size_t                          text_cursor_pos{0};
	double		                dt{0.0};
	const char*			window_title{"WeeabooPoint"};
	bool				waiting_for_input{false};

	GameState() = default;
};



// callbacks
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void gamestate_character_callback(GLFWwindow* window, unsigned int codepoint);
void gamestate_key_callback(GLFWwindow* window, int key, int seconds, int action, int mods);
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_framebuffer_size_callback(GLFWwindow* window, int width, int height);
