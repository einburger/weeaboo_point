#include "animation_controller.h"
#include "graphing.h"
#include "gamestate.h"
#include "geometry.h"

void AnimationWindow::update_and_draw()
{
	canvas_p = ImGui::GetCursorScreenPos();
	canvas_sz = ImGui::GetContentRegionAvail();

	canvas_sz.y = 300.0f;

	mouse_pos_global = ImGui::GetIO().MousePos;
	mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x,
							  mouse_pos_global.y - canvas_p.y);

	draw_list = ImGui::GetWindowDrawList();

	ImGui::InvisibleButton("canvas", canvas_sz);

	draw_list->AddRectFilledMultiColor(canvas_p,
									   ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y),
									   IM_COL32(50, 50, 50, 255),
									   IM_COL32(50, 50, 60, 255),
									   IM_COL32(60, 60, 70, 255),
									   IM_COL32(50, 50, 60, 255));

	draw_list->AddRect(canvas_p,
					   ImVec2(canvas_p.x + canvas_sz.x,
					   canvas_p.y + canvas_sz.y),
					   IM_COL32(255, 255, 255, 255));

	draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);


	for (auto& p : animation_line)
	{
		p.x += canvas_p.x;
		p.y += canvas_p.y;
		draw_list->AddCircle(p, 2, IM_COL32(255, 0, 0, 255), 5, 1);
	}

	for (auto p : control_points)
	{
		p.x += canvas_p.x;
		p.y += canvas_p.y;
		draw_list->AddCircleFilled(p, 5, IM_COL32(255, 255, 0, 255), 12);
	}

	// halfwayline
	draw_list->AddLine(ImVec2(canvas_p.x, canvas_p.y + (canvas_sz.y / 2)),
					   ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + (canvas_sz.y / 2)),
					   IM_COL32(255, 255, 255, 100));

	draw_list->PopClipRect();
}


void AnimationController::update_and_draw()
{
	auto initialize_control_points = [&]
	{
		step_size = window.canvas_sz.x / ((float)number_of_control_points - 1);
		static float canvas_start_x = canvas_size_x();
		if (window.control_points.empty() || canvas_start_x != canvas_size_x())
		{
			canvas_start_x = canvas_size_x();
			window.control_points.clear();
			window.animation_line.clear();
			for (int i{ -1 }; i < number_of_control_points + 1; ++i)
			{ // we need two points off screen on both sides so index at -1
				window.control_points.push_back(ImVec2(i * step_size, window.canvas_sz.y / 2));
			}
		}
	};

	auto process_control_point_dragging = [&]
	{
		static bool dragging_point = false;
		static ImVec2* drag_point = &window.control_points[0];
		static int left_x_bound = window.control_points[0].x, right_x_bound = window.control_points.back().x;
		if (ImGui::IsMouseDragging(0) && dragging_point)
		{

			auto in_drag_point_bounds = [&]
			{
				return drag_point->x >= left_x_bound && drag_point->x <= right_x_bound;
			};


			if (drag_point != &window.control_points[1]
				&& drag_point != &window.control_points[window.control_points.size() - 2])
			{ // first point and last point are dummy points, want to fix the second and second to last points
				if (drag_point->x < left_x_bound)
				{
					if (ImGui::GetMouseDragDelta().x < 0)
						return;
				}
				if (drag_point->x > right_x_bound)
				{
					if (ImGui::GetMouseDragDelta().x > 0)
						return;
				}

				drag_point->x = mouse_pos_local_x();
			}

			if (drag_point == &window.control_points[1])
				window.control_points[1] = *drag_point;
			if (drag_point == &window.control_points[window.control_points.size() - 2])
				window.control_points[window.control_points.size() - 2] = *drag_point;
			drag_point->y = mouse_pos_local_y();

		}
		if (ImGui::IsMouseClicked(0) && !dragging_point)
		{
			int i = 0;
			for (auto& control_point : window.control_points)
			{
				if (std::abs(mouse_pos_local_x() - control_point.x) < 7 &&
					std::abs(mouse_pos_local_y() - control_point.y) < 7)
				{
					left_x_bound = i - 1 > 0 ? window.control_points[i - 1].x
						: window.control_points[1].x;
					right_x_bound = i + 1 < window.control_points.size() - 2 ? window.control_points[i + 1].x
						: window.control_points[window.control_points.size() - 2].x;
					dragging_point = true;
					drag_point = &control_point;
				}
				i++;
			}
		}
		if (ImGui::IsMouseReleased(0))
		{
			dragging_point = false;
		}
	};

	auto setup_animation_controls = [&]
	{
		if (ImGui::SliderInt("Number Of Control Points", &number_of_control_points,
			MIN_CONTROL_POINTS, MAX_CONTROL_POINTS))
		{
			window.control_points.clear();
			step_size = window.canvas_sz.x / ((float)number_of_control_points - 1);
		}

		if (ImGui::SliderInt("Playback Speed", &window.xy_screen_scale_y, MAX_TIME_STEPS, MIN_TIME_STEPS, ""))
		{
		}

		if (current_step < window.animation_line.size())
		{
			window.draw_list->AddLine(ImVec2(window.animation_line[current_step].x + window.canvas_p.x, window.canvas_p.y),
									  ImVec2(window.animation_line[current_step].x + window.canvas_p.x, window.canvas_p.y + window.canvas_sz.y),
									  IM_COL32(255, 0, 0, 255));

			selected_character->set_x(local_to_game_x(window.animation_line[current_step].y));
		}
		else { current_step = 0; }



		/*
		if (ImGui::Button("Reset")) {
			control_points.clear();
		} ImGui::SameLine();

		if (ImGui::Button("Play"))
		{
			GameState::waiting_for_input = false;
			auto spl = get_catmull_rom(control_points, time_steps);
			for (auto& p : spl) { p = (window.canvas_sz.x - p) / canvas_sz.x * GameState::w_h.x; }
			//event_handler->push_back(new MoveEvent(&ch, spl));
		}
		if (ImGui::Button("Save"))
		{
			GameState::scene.fns[GameState::restore_point] =
				[&ch, spline] { event_handler->push_back(new MoveEvent(&ch, spline)); };
		}
		*/
	};


	if (!selected_character) { selected_character = &GameState::scene.characters[0]; }
	window.update_and_draw();
	initialize_control_points();
	process_control_point_dragging();

	static int char_index = 0;
	const char* charitems = "sunohara\0nagisa\0\0";
	if (ImGui::Combo("Characters", &char_index, charitems))
	{
		selected_character = &GameState::scene.characters[char_index];
	}

	static int interp_index = 0;
	const char* interpitems = "linear\0step\0spline\0sin\0\0";
	ImGui::Combo("Interpolation", &interp_index, interpitems);
	switch (interp_index)
	{
	case 0:
		//window.animation_line = get_linear_drawn(window.control_points, time_steps);
		window.animation_line = get_linear_drawn(window.control_points, window.canvas_sz.x);
		break;
	case 1:
		window.animation_line = get_step_drawn(window.control_points, window.canvas_sz.x);
		break;
	case 2:
		window.animation_line = get_catmull_rom_drawn(window.control_points, window.canvas_sz.x);
		break;
	case 3:
		static int amplitude=0;
		ImGui::SliderInt("Amplitude", &amplitude, 0, window.canvas_sz.y);
		window.animation_line = get_sin_drawn(window.control_points, window.canvas_sz.y, amplitude);
		break;
	}

	setup_animation_controls();

}



float AnimationController::mouse_pos_local_x() { return window.mouse_pos_canvas.x; }
float AnimationController::mouse_pos_local_y() { return window.mouse_pos_canvas.y; }
float AnimationController::mouse_pos_global_x() { return window.mouse_pos_global.x; }
float AnimationController::mouse_pos_global_y() { return window.mouse_pos_global.y; }
float AnimationController::canvas_size_x() { return window.canvas_sz.x; };
float AnimationController::canvas_size_y() { return window.canvas_sz.y; };
float AnimationController::local_to_game_x(float value) { return ((window.canvas_sz.y - value) / window.canvas_sz.y) * GameState::w_h.x; }
float AnimationController::local_to_game_y(float value) { return ((window.canvas_sz.y - value) / window.canvas_sz.y) * GameState::w_h.y; }
