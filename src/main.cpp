// devildoctor.cpp : Defines the entry point for the console application.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#include <string>

#include "globals.h" // PATHS
#include "fileloader.h"

std::unique_ptr<EventHandler> event_handler(new EventHandler());

size_t line_number = 0;
size_t target_line = 0;

#include "scene.h" // scene_create

int main()
{
	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_SAMPLES, 8);


	GameState::current_window = glfwCreateWindow(GameState::w_h[0], GameState::w_h[1],
												 GameState::window_title, NULL, NULL);

	if (!GameState::current_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(GameState::current_window);
	//glfwSwapInterval(1); // vsync


	std::string path = SCRIPT_PATH + std::string{ "Text.txt" };

	GameState::scene = Scene(path);

	GameState::dt = 0.001;
	event_handler->push_back(new Event<ParseEvent>);

	glfwSetFramebufferSizeCallback(GameState::current_window, gamestate_framebuffer_size_callback);
	glfwSetWindowSizeCallback(GameState::current_window, gamestate_window_size_callback);
	glfwSetKeyCallback(GameState::current_window, gamestate_key_callback);
	glfwSetCharCallback(GameState::current_window, gamestate_character_callback);
	glfwSetMouseButtonCallback(GameState::current_window, gamestate_mouse_button_callback);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, GameState::w_h[0], GameState::w_h[1], 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// fileloader::load_images();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(GameState::current_window, true);
	ImGui_ImplOpenGL2_Init();

	// target_line= GameState::scene.script.size()-1;
	target_line = 2;
	while (!glfwWindowShouldClose(GameState::current_window))
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

			// ImGui::Text("Avg fps %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			//  if (ImGui::Button("Save Changes")) {
			//      scene_states->update_state(line_number);
			//  }

			auto& scene = GameState::scene;

			static unsigned int start_idx = 0, end_idx = scene.background.sprite_paths.size() - 1;
			if (ImGui::SliderScalar("Backgrounds", ImGuiDataType_U32, &scene.background.sprite_paths_idx,
				&start_idx, &end_idx, ""))
			{
				scene.background.set_texture(scene.background.sprite_paths[scene.background.sprite_paths_idx]);
			}

			ImGui::Separator();

			if (ImGui::TreeNode("Characters"))
			{

				for (auto& ch : GameState::scene.characters)
				{
					if (ImGui::TreeNode(ch.name.c_str()))
					{

						unsigned int start_idx_2 = 0, end_idx_2 = ch.sprite_paths.size() - 1;
						if (ImGui::SliderScalar("Sprites", ImGuiDataType_U32, &ch.sprite_paths_idx, &start_idx_2, &end_idx_2, ""))
						{
							ch.set_texture(ch.sprite_paths[ch.sprite_paths_idx]);
						}


						ImGui::SliderInt("X-Pos", &ch.min_xy[0], -ch.w_h[0], GameState::w_h[0], "");
						ImGui::SliderInt("Y-Pos", &ch.min_xy[1], GameState::w_h[1], -ch.w_h[1], "");
						ImGui::ColorEdit4("Color", ch.rgba.data());

						ch.set_pos(ch.min_xy[0], ch.min_xy[1]);

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::BeginChild("Scrolling");

			int width = ImGui::GetWindowWidth();
			for (int i{}; i < line_number; ++i)
			{
				if (ImGui::Button(GameState::scene.script[i].c_str(), ImVec2(width, 20)))
				{
					//event_handler->events.clear();
					//GameState::scene.textfield.text_cursor_pos = GameState::scene.dialog.size();
					//GameState::waiting_for_input = true;
				}
			}
			ImGui::EndChild();

			ImGui::End();
		}

		ImGui::Render();

		GameState::scene.draw();

		if (!GameState::waiting_for_input)
			event_handler->process();


		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		glFlush();

		glfwSwapBuffers(GameState::current_window);
		glfwPollEvents();

		glDisable(GL_TEXTURE_2D);
	}

	glfwTerminate();

	return 0;
}
