#pragma once
#include "eventhandler.h"

extern struct Scene* scene; //forward declaration
extern struct Box* box;

typedef struct CursorState {
	char holding_left_mouse; 
	char clicked;
	double current_x
		 , current_y
		 , click_pos_x
		 , click_pos_y;
} CursorState;

typedef struct GameState {
	FILE*               current_file;
	GLFWwindow*         current_window;
	Box*				last_clicked_box;
	struct Scene*		scene;
	int                 window_width;
	int                 window_height;
	int			        fps;
	size_t              text_cursor_pos;
	double		        dt;
	const char*         window_title;
	char				waiting_for_input;
} GameState;

typedef struct SaveState {
	int box_pos_x
	  , box_pos_y
	  , box_width
	  , box_height
	  , window_width
	  , window_height;
} SaveState;


// callbacks
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void gamestate_character_callback(GLFWwindow* window, unsigned int codepoint);
void gamestate_key_callback(GLFWwindow* window, int key, int seconds, int action, int mods);
void gamestate_window_size_callback(GLFWwindow* window, int width, int height);
void gamestate_framebuffer_size_callback(GLFWwindow* window, int width, int height);

void gamestate_savestate_update(int x, int y, int w, int h, int window_w, int window_h);
void gamestate_savestate_save(SaveState* save_state);
void gamestate_savestate_load(SaveState* save_state);