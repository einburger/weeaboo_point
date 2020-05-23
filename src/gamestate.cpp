#include <GLFW/glfw3.h>

#include "globals.h" // game_state

void gamestate_window_size_callback(GLFWwindow* window, int width, int height)
{
	game_state->window_height = height;
	game_state->window_width = width;

	// resize everything in the scene
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
			game_state->text_cursor_pos = 0;
			game_state->waiting_for_input = false;
			event_handler->events.clear();
		}
	}
}

