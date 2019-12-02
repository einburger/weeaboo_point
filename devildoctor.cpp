// devildoctor.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include "globals.h"
#include "geometry.h"
#include "scene.h"
#include "parser.h"
#include "text.h"
#include "eventhandler.h"
#include "memorypool.h"
#include <assert.h>

int main(int argc, char** argv)
{
	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_SAMPLES, 8);

	game_state->current_window = glfwCreateWindow(game_state->window_width, 
												  game_state->window_height, 
												  game_state->window_title, 
												  NULL, 
												  NULL);

	if (!game_state->current_window)
	{
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(game_state->current_window);
	glfwSwapInterval(1); // vsync

	allocate_pool(&memory_pool, 500'000'000);

	game_state->scene = scene_create(CONCAT(SCRIPT_PATH, "Text.txt"));

	game_state->dt = 0.001;
	eventhandler_event_create(PARSE, NULL);

	glfwSetFramebufferSizeCallback(game_state->current_window, gamestate_framebuffer_size_callback);
	glfwSetWindowSizeCallback(game_state->current_window, gamestate_window_size_callback);
	glfwSetKeyCallback(game_state->current_window, gamestate_key_callback);
	glfwSetCharCallback(game_state->current_window, gamestate_character_callback);
	glfwSetMouseButtonCallback(game_state->current_window, gamestate_mouse_button_callback);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, game_state->window_width, game_state->window_height, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(game_state->current_window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		scene_draw();

		eventhandler_event_handle();

		glFlush();

		glfwSwapBuffers(game_state->current_window);
		glfwPollEvents();

		glDisable(GL_TEXTURE_2D);
	}

	free(memory_pool.base);
	glfwTerminate();
	return 1;
}
