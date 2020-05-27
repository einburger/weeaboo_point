#include <string>
#include <functional>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include "globals.h" // PATHS
#include "fileloader.h"
#include "graphing.h"
#include "eventtypes.h"
#include "eventhandler.h"
#include "utils.h"

std::unique_ptr<EventHandler> event_handler(new EventHandler());

size_t line_number = 0;
size_t target_line = 0;
size_t restore_point = 0;

#include "scene.h" // scene_create

int main()
{
	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_SAMPLES, 8);


	GameState::current_window = glfwCreateWindow(GameState::w_h.x, GameState::w_h.y,
												 GameState::window_title, NULL, NULL);

	if (!GameState::current_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(GameState::current_window);
	glfwSwapInterval(0); // vsync

	std::string path = SCRIPT_PATH + std::string{ "Text.txt" };

	GameState::scene = Scene(path);
	event_handler->push_back(new ParseEvent());

	glfwSetFramebufferSizeCallback(GameState::current_window, gamestate_framebuffer_size_callback);
	glfwSetWindowSizeCallback(GameState::current_window, gamestate_window_size_callback);
	glfwSetKeyCallback(GameState::current_window, gamestate_key_callback);
	glfwSetCharCallback(GameState::current_window, gamestate_character_callback);
	glfwSetMouseButtonCallback(GameState::current_window, gamestate_mouse_button_callback);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, GameState::w_h.x, GameState::w_h.y, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(GameState::current_window, true);
	ImGui_ImplOpenGL2_Init();

	while (!glfwWindowShouldClose(GameState::current_window))
	{
		GameState::curr_time = glfwGetTime();
		GameState::dt += (GameState::curr_time - GameState::prev_time) / GameState::fps;
		GameState::prev_time = GameState::curr_time;

		while (GameState::dt >= 1.0)
		{
			if (!GameState::waiting_for_input)
				event_handler->process();
			GameState::dt--;
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ImGui::Begin("Editor");
		if (ImGui::BeginTabBar("Oogie"))
		{
			if (ImGui::BeginTabItem("Editor"))
			{
				static unsigned int start_idx = 0, end_idx = GameState::scene.background.sprite_paths.size() - 1;
				if (ImGui::SliderScalar("Backgrounds", ImGuiDataType_U32, &GameState::scene.background.sprite_paths_idx,
					&start_idx, &end_idx, ""))
				{
					GameState::scene.background.set_texture(GameState::scene.background.sprite_paths[GameState::scene.background.sprite_paths_idx]);
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

							ImGui::ColorEdit4("Color", ch.rgba.data());
							ImGui::Text("width height %d %d", ch.w_h.x, ch.w_h.y);
							ImGui::Text("x y %d %d", ch.center.x, ch.center.y);

							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}

				ImGui::Separator();
				ImGui::BeginChild("Scrolling");

				int width = ImGui::GetWindowWidth();
				for (int i{ 1 }; i < line_number; ++i)
				{
					if (ImGui::Button(GameState::scene.script[i].c_str(), ImVec2(width, 20)))
					{
						restore_point = i - 1;
						GameState::scene.restore(i-1);
					}
				}
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Animator"))
			{
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 canvas_p = ImGui::GetCursorScreenPos();
				ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
				canvas_sz.y = 300.0f; canvas_sz.x = 500.0f;
				if (canvas_sz.x < 500.0f) canvas_sz.x = 500.0f;
				if (canvas_sz.y < 300.0f) canvas_sz.y = 300.0f;
				ImVec2 mouse_pos_global = ImGui::GetIO().MousePos;
				ImVec2 mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, 
												 mouse_pos_global.y - canvas_p.y);
				static ImVector <ImVec2> spline_control_points{};
				ImVector <ImVec2> spline{};
				static int number_of_control_points = 2;
				static int time_slices = 100; // controls speed of animation also represents frames of animation
				static float step_sz = canvas_sz.x / ((float)number_of_control_points - 1); // length betwen each control point
				static int current_step = 0; // current animation time slice (animation frame)

				auto draw_canvas_bg = [&] {
					ImGui::InvisibleButton("canvas", canvas_sz);

					draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y),
													   IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));

					draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y),
									   IM_COL32(255, 255, 255, 255));
				};

				auto initialize_bezier_control_points = [&] {
					if (spline_control_points.empty())
					{
						for (int i{ -1 }; i < number_of_control_points + 1; ++i)
						{ // we need two points off screen on both sides so index at -1
							spline_control_points.push_back(ImVec2(0 + (i * step_sz), canvas_sz.y / 2));
						}
					}
				};

				auto process_control_point_dragging = [&] {
					static bool dragging_point = false;
					static ImVec2* drag_point = &spline_control_points[0];
					if (ImGui::IsMouseDragging(0) && dragging_point)
					{
						if (drag_point != &spline_control_points[1] 
							&& drag_point != &spline_control_points[spline_control_points.size() - 2])
						{ // first point and last point are dummy points, want to fix the second and second to last points
							drag_point->x = mouse_pos_canvas.x;
						}
						if (drag_point == &spline_control_points[1])
							spline_control_points[1] = *drag_point;
						if (drag_point == &spline_control_points[spline_control_points.size()-2])
							spline_control_points[spline_control_points.size()-2] = *drag_point;
						drag_point->y = mouse_pos_canvas.y;

					}
					if (ImGui::IsMouseDown(0) && !dragging_point)
					{
						for (auto &control_point : spline_control_points)
						{
							if (std::abs(mouse_pos_canvas.x - control_point.x) < 7 &&
								std::abs(mouse_pos_canvas.y - control_point.y) < 7)
							{
								dragging_point = true;
								drag_point = &control_point;
							}
						}
					}
					if (ImGui::IsMouseReleased(0))
					{
						dragging_point = false;
					}
				}; 

				auto draw_animation_graph = [&]
				{
					draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);
					draw_list->AddLine(ImVec2(canvas_p.x + 0, canvas_p.y + (canvas_sz.y / 2)),
									   ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + (canvas_sz.y / 2)),
									   IM_COL32(255, 255, 255, 255), 1);

					for (auto &p : spline)
					{
						p.x += canvas_p.x;
						p.y += canvas_p.y;
						draw_list->AddCircleFilled(p, 2, IM_COL32(0, 200, 200, 200), 5);
					}
					draw_list->AddPolyline(spline.Data, spline.size(), IM_COL32(0, 0, 0, 100), false, 2);

					for (auto p : spline_control_points)
					{
						p.x += canvas_p.x;
						p.y += canvas_p.y;
						draw_list->AddCircle(p, 5, IM_COL32(255, 0, 0, 255), 5, 5);
					}

					draw_list->PopClipRect();

				};

				auto setup_animation_sliders = [&] {
					ImGui::Separator();
					if (ImGui::SliderInt("Number Of Control Points", &number_of_control_points, 2, 10)) {
						spline_control_points.clear();
						step_sz = canvas_sz.x / ((float)number_of_control_points - 1);
					}
					ImGui::SliderInt("Animation Time Steps", &time_slices, 1, 500, "");

					Character &ch = GameState::scene.characters[1];
					if (ImGui::SliderInt("Preview Step", &current_step, 0, time_slices)) {
						if (current_step < spline.size())
							ch.set_x(((300.0f - spline[current_step].y) / 300.0f) * GameState::w_h.x);
					}

					if (ImGui::Button("Reset")) {
						spline_control_points.clear(); 
					} ImGui::SameLine();

					if (ImGui::Button("Play"))
					{
						event_handler->events.clear();
						GameState::waiting_for_input = false;
						event_handler->push_back(new MoveEvent(&ch, spline));
					}
					if (ImGui::Button("Save"))
					{
						GameState::scene.fns[restore_point] =
							[&ch, spline] { event_handler->push_back(new MoveEvent(&ch, spline)); };
					}
				};

				draw_canvas_bg();
				initialize_bezier_control_points();
				process_control_point_dragging();
				spline = get_catmull_rom(spline_control_points, time_slices);
				draw_animation_graph();
				setup_animation_sliders();

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();


		ImGui::Render();

		GameState::scene.draw();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());


		glFlush();
		glfwSwapBuffers(GameState::current_window);
		glfwPollEvents();

		glDisable(GL_TEXTURE_2D);
	}

	glfwTerminate();

	return 0;
}
