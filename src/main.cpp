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



ImVector<ImVec2> get_bez(ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3, int time_slices)
{
	auto cube_bez = [](float a, float b, float c, float d, float t)
	{
		auto lerp = [](float a, float b, float t)
		{
			return a * (1.0f - t) + b * t;
		};
		auto quadric_bez = [lerp](float a, float b, float c, float t)
		{
			return lerp(lerp(a, b, t), lerp(b, c, t), t);
		};
		return lerp(quadric_bez(a, b, c, t), quadric_bez(b, c, d, t), t);
	};

	ImVector<ImVec2> vec;
	for (int i = 0; i < time_slices; ++i)
	{
		float t = ((float)i) / ((float)time_slices);
		float xp = cube_bez(p0.x, p1.x, p2.x, p3.x, t);
		float yp = cube_bez(p0.y, p1.y, p2.y, p3.y, t);
		vec.push_back(ImVec2(xp, yp));
	}

	return vec;
}

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

		ImGui::Begin("Editor");

		auto& scene = GameState::scene;

		if (ImGui::BeginTabBar("##TabVar"))
		{
			if (ImGui::BeginTabItem("Editor"))
			{
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

				ImGui::EndTabItem();
			}
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			if (ImGui::BeginTabItem("Animator"))
			{
				static ImVector <ImVec2> points{};
				static bool adding_line = false;



				ImVec2 canvas_p = ImGui::GetCursorScreenPos();
				ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
				canvas_sz.y = 300.0f;
				if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
				draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), 
												   IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
				draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), 
								   IM_COL32(255, 255, 255, 255));


				bool adding_preview = false;
				ImGui::InvisibleButton("canvas", canvas_sz);
				ImVec2 mouse_pos_global = ImGui::GetIO().MousePos;
				ImVec2 mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, 
												 mouse_pos_global.y - canvas_p.y);

				if (points.empty())
				{
					static float fourth = canvas_sz.x / 3;
					points.push_back(ImVec2(0, canvas_sz.y / 2));
					points.push_back(ImVec2(fourth, canvas_sz.y / 2));
					points.push_back(ImVec2(fourth + fourth, canvas_sz.y / 2));
					points.push_back(ImVec2(canvas_sz.x, canvas_sz.y / 2));
				}

				static bool dragging_point = false;
				static ImVec2 *drag_point = &points[1];
				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDragging(0) && dragging_point)
					{
							if (drag_point != &points[0] && drag_point != &points[3])
								drag_point->x = mouse_pos_canvas.x;
							drag_point->y = mouse_pos_canvas.y;
					}
					if (ImGui::IsMouseDown(0) && !dragging_point)
					{
						for (int i{}; i < points.Size; ++i)
						{
							if (std::abs(mouse_pos_canvas.x - points[i].x) < 7 &&
								std::abs(mouse_pos_canvas.y - points[i].y) < 7)
							{
								dragging_point = true;
								drag_point = &points[i];
							}
						}
					}
				}
				if (ImGui::IsMouseReleased(0))
				{
					dragging_point = false;
				}

				static int time_slices = 10;
				ImGui::SliderInt("Animation Time Steps", &time_slices, 4, 100); 
				if (ImGui::Button("Reset")) points.clear();
				draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);
				draw_list->AddLine(ImVec2(canvas_p.x + 0, canvas_p.y + (canvas_sz.y / 2)), 
								   ImVec2(canvas_p.x +  canvas_sz.x, canvas_p.y+ (canvas_sz.y / 2)),
								   IM_COL32(255,255,255,255), 1);
				if (points.Size == 4)
				{
					ImVec2 a(canvas_p.x + points[0].x, canvas_p.y + points[0].y);
					ImVec2 b(canvas_p.x + points[1].x, canvas_p.y + points[1].y);
					ImVec2 c(canvas_p.x + points[2].x, canvas_p.y + points[2].y);
					ImVec2 d(canvas_p.x + points[3].x, canvas_p.y + points[3].y);
					// auto llll = get_bez(a, b, c, d, time_slices);
					for (auto cc : get_bez(a,b,c,d,time_slices))
					{
						draw_list->AddCircleFilled(cc, 4, IM_COL32(0, 255, 0, 100), 12);
					}
					draw_list->AddCircleFilled(a, 7, IM_COL32(255, 0, 0, 255), 12);
					draw_list->AddCircleFilled(b, 7, IM_COL32(255, 0, 0, 255), 12);
					draw_list->AddCircleFilled(c, 7, IM_COL32(255, 0, 0, 255), 12);
					draw_list->AddCircleFilled(d, 7, IM_COL32(255, 0, 0, 255), 12);
				}
				draw_list->PopClipRect();

				
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();

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
