// devildoctor.cpp : Defines the entry point for the console application.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#include <string>

#include "globals.h" // PATHS

std::unique_ptr<GameState>    game_state(new GameState());
std::unique_ptr<CursorState>  cursor(new CursorState());
std::unique_ptr<EventHandler> event_handler(new EventHandler());
std::unique_ptr<SceneStateStack> scene_states(new SceneStateStack);

size_t line_number = 0;

#include "scene.h" // scene_create

int main()
{
	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_SAMPLES, 8);


        game_state->current_window = glfwCreateWindow(game_state->window_width, 
                game_state->window_height, 
                game_state->window_title, 
                NULL, 
                NULL);

	if (!game_state->current_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(game_state->current_window);
	//glfwSwapInterval(1); // vsync

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(game_state->current_window, true);
        ImGui_ImplOpenGL2_Init();

	std::string path = SCRIPT_PATH + std::string{"Text.txt"};
	game_state->scene = Scene(path);

	game_state->dt = 0.001;
	event_handler->push_back(new Event<ParseEvent>);

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

                ImGui_ImplOpenGL2_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                {
                    ImGui::Begin("Editor");

                    ImGui::Text("Avg fps %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                                                                        ImGui::GetIO().Framerate);
                    ImGui::Separator();

                    if (ImGui::TreeNode("Characters")) {

                        for (auto &ch : game_state->scene.characters) {
                            if (ImGui::TreeNode(ch.name.c_str())) {

                                ImGui::SliderInt("X-Pos", &ch.min_xy[0], -ch.w_h[0], game_state->window_width);
                                ch.set_position(ch.min_xy);

                                // int xx = new_x;

                                ImGui::SliderFloat("Alpha", &ch.rgba[3], 0.0f, 1.0f);

                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }

                    ImGui::Separator();
                    ImGui::BeginChild("Scrolling");

                    int width = ImGui::GetWindowWidth();
                    for (int i{}; i < line_number; ++i) {
                        if (ImGui::Button(game_state->scene.script[i].c_str(), ImVec2(width, 20))) {
                                event_handler->events.clear();
                                scene_states->revert_state(i);
                        }
                    }
                    ImGui::EndChild();
                    ImGui::End();
                }
                
                ImGui::Render();

		game_state->scene.draw();

		if (!game_state->waiting_for_input)
			event_handler->process();

		glFlush();

                ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(game_state->current_window);
		glfwPollEvents();

		glDisable(GL_TEXTURE_2D);
	}

	glfwTerminate();

	return 0;
}
