#include <GLFW/glfw3.h>
#include <string>

#include "gamestate.h"
#include "eventhandler.h"

GLFWwindow*				GameState::current_window=nullptr;
Scene					GameState::scene{};
std::array<int, 2>		GameState::w_h{1920, 1080};
double					GameState::fps{ 1.0 / 60 };
double					GameState::dt{ 0.0 };
double					GameState::prev_time{ glfwGetTime() };
double					GameState::curr_time{ 0.0 };
const char*				GameState::window_title{ "WeeabooPoint" };
bool					GameState::waiting_for_input{ false };
int						GameState::text_cursor_pos{};


void gamestate_window_size_callback(GLFWwindow* window, int width, int height)
{
	GameState::w_h = std::array{ width, height };
}

void gamestate_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void gamestate_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) { }

void gamestate_character_callback(GLFWwindow* window, unsigned int codepoint) { }

void gamestate_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!GameState::waiting_for_input)
		return;

	if (key == GLFW_KEY_Z || key == GLFW_KEY_SPACE || key == GLFW_KEY_TAB)
	{
		if (action == GLFW_PRESS)
		{
			GameState::text_cursor_pos = 0;
			GameState::scene.dialog = "";
			GameState::waiting_for_input = false;
			event_handler->events.clear();
		}
	}
}

