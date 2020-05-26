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

ImVector<ImVec2> get_bez_alt(ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3, int time_slices)
{
	auto bez = [](float a, float b, float c, float d, float t)
	{
		const float C1 = (d - (3.0f * c) + (3.0f * b) - a);
		const float C2 = ((3.0f * c) - (6.0f * b) + (3.0f * a));
		const float C3 = ((3.0f * b) - (3.0f * a));
		const float C4 = a;

		return (C1 * t * t * t) + (C2 * t * t) + (C3 * t) + C4;
	};

	ImVector<ImVec2> vec;
	ImVec2 point;
	float target_delta = 500.0f / time_slices;

	double dx = 0.005f;
	double prev_t = 0.0f;
	while (prev_t < 1.0f)
	{
		float temp_x_curr = bez(p0.x, p1.x, p2.x, p3.x, prev_t);
		do {
			prev_t += dx;
		} while (std::abs(bez(p0.x, p1.x, p2.x, p3.x, prev_t) - temp_x_curr) < target_delta);
		point.x = bez(p0.x, p1.x, p2.x, p3.x, prev_t);
		point.y = bez(p0.y, p1.y, p2.y, p3.y, prev_t);
		vec.push_back(point);
	}

	return vec;
}

ImVector<ImVec2> get_catmull_rom(ImVector<ImVec2> &points, int time_slices)
{
	if (points.Size < 4) {
		return {};
	}

	ImVec2 point;
	ImVector<ImVec2> vec;
	const float target_delta = 500.0f / time_slices;

	auto catmull_rom = [](float a, float b, float c, float d, float t)
	{
		return 0.5f * ((-a + 3 * b - 3 * c + d) * t * t * t
					  + (2 * a - 5 * b + 4 * c - d) * t * t
					  + (-a + c) * t
					  + 2 * b);
	};

	auto uniform_catmull_rom = [&](ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3)
	{
		double dx = 0.005f;
		double prev_t = 0.0f;
		while (prev_t < 1.0f)
		{
			float temp_x_curr = catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t);
			do
			{
				prev_t += dx;
			} while (std::abs(catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t)) - temp_x_curr < target_delta);
			point.x = catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t);
			point.y = catmull_rom(p0.y, p1.y, p2.y, p3.y, prev_t);
			vec.push_back(point);
		}
		vec.pop_back(); // don't want duplicates
	};

	for (int i{ 3 }; i < points.Size; ++i)
	{
		uniform_catmull_rom(points[i-3], points[i-2], points[i-1], points[i]);
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
				static bool adding_line = false;

				ImVec2 canvas_p = ImGui::GetCursorScreenPos();
				ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
				canvas_sz.y = 300.0f;
				canvas_sz.x = 500.0f;
				//if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
				//if (canvas_sz.y < 300.0f) canvas_sz.y = 300.0f;
				draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), 
												   IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
				draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), 
								   IM_COL32(255, 255, 255, 255));


				bool adding_preview = false;
				ImGui::InvisibleButton("canvas", canvas_sz);
				ImVec2 mouse_pos_global = ImGui::GetIO().MousePos;
				ImVec2 mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, 
												 mouse_pos_global.y - canvas_p.y);

				static ImVector <ImVec2> points{};
				static int number_of_cpoints = 10;
				static float step_sz = canvas_sz.x / ((float)number_of_cpoints - 1);
				if (ImGui::SliderInt("Number Of Control Points", &number_of_cpoints, 4, 10))
				{
					points.clear();
					step_sz = canvas_sz.x / ((float)number_of_cpoints - 1);
				}
				if (points.empty())
				{
					for (int i{ -1 }; i < number_of_cpoints+1; ++i)
					{ // we need two points off screen on both sides so index at -1
						points.push_back(ImVec2(0 + (i * step_sz), canvas_sz.y / 2));
					}
				}

				static bool dragging_point = false;
				static ImVec2 *drag_point = &points[0];
				if (1)
				{
					if (ImGui::IsMouseDragging(0) && dragging_point)
					{
						if (drag_point != &points[1] && drag_point != &points[points.size()-2])
						{ // first point and last point are dummy points, want to fix the second and second to last points
							drag_point->x = mouse_pos_canvas.x;
						}
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

				Character *ch = &GameState::scene.characters[0];

				static int time_slices = 200, current_step=0;
				ImGui::SliderInt("Animation Time Steps", &time_slices, 4, 500); 
				draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);
				draw_list->AddLine(ImVec2(canvas_p.x + 0, canvas_p.y + (canvas_sz.y / 2)), 
								   ImVec2(canvas_p.x +  canvas_sz.x, canvas_p.y+ (canvas_sz.y / 2)),
								   IM_COL32(255,255,255,255), 1);


				auto spline = get_catmull_rom(points, time_slices);

				for (auto p : spline)
				{
					p.x += canvas_p.x;
					p.y += canvas_p.y;
					draw_list->AddCircleFilled(p, 2, IM_COL32(0, 255, 255, 100), 5);
				}

				for (auto p : points)
				{
					p.x += canvas_p.x;
					p.y += canvas_p.y;
					draw_list->AddCircle(p, 5, IM_COL32(255, 0, 0, 255), 5);
				}


				draw_list->PopClipRect();

				if (ImGui::SliderInt("Preview Step", &current_step, 0, time_slices))
				{
					if (current_step < spline.size())
						ch->set_x(((300.0f - spline[current_step].y) / 300.0f) * GameState::w_h[0]);
				}
				
				if (ImGui::Button("Reset")) points.clear();
				if (ImGui::Button("Play"))
				{
					event_handler->events.clear();
					GameState::waiting_for_input = false;
					event_handler->push_back(new Event<MoveGraph>(std::tuple<Character *, 
											 ImVector<ImVec2>>(ch, spline)));
					line_number--;
				}

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
