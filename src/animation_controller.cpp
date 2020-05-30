#include "animation_controller.h"
#include "graphing.h"
#include "gamestate.h"
#include "geometry.h"

static int field_index = X_POSITION;
static int interp_index = LINEAR;

ImU32 line_colors_xx[6] = {
	IM_COL32(0,255,255,255),
	IM_COL32(255,0,255,255),
	IM_COL32(255,0,0,255),
	IM_COL32(0,255,0,255),
	IM_COL32(0,0,255,255),
	IM_COL32(255,255,255,255)
};
void AnimationWindow::pre_update()
{
	canvas_p = ImGui::GetCursorScreenPos();
	canvas_sz = ImGui::GetContentRegionAvail();

	canvas_sz.y = 300.0f;
	canvas_sz.x = FRAME_COUNT;

	mouse_pos_global = ImGui::GetIO().MousePos;
	mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, mouse_pos_global.y - canvas_p.y);

	draw_list = ImGui::GetWindowDrawList();

	ImGui::InvisibleButton("canvas", canvas_sz);
}

void AnimationWindow::draw_window()
{
	draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y),
									   IM_COL32(50, 50, 50, 255),
									   IM_COL32(50, 50, 60, 255),
									   IM_COL32(60, 60, 70, 255),
									   IM_COL32(50, 50, 60, 255));

	draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y),
					   IM_COL32(255, 255, 255, 255));

	draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);

	int idx = current_frame_positions[field_index];
	draw_list->AddLine(ImVec2(interpolated_x[field_index][idx] + canvas_p.x, canvas_p.y),
					ImVec2(interpolated_x[field_index][idx] + canvas_p.x, canvas_p.y + canvas_sz.y),
					IM_COL32(255, 0, 0, 255));


	for (int i{}; i < NUM_OF_FIELDS; ++i)
	{
		if (i == field_index)
			continue;
		for (int frame_i{}; frame_i < interpolated_x[i].size(); ++frame_i)
		{
			float x = frame_i + canvas_p.x;
			float y = interpolated_y[i][frame_i] + canvas_p.y;
			draw_list->AddCircle(ImVec2(x, y), 1.0, line_colors_xx[i], 5, 0.1);
		}
	}

	for (int frame_i{}; frame_i < interpolated_x[field_index].size(); ++frame_i)
	{
		float x = frame_i + canvas_p.x;
		float y = interpolated_y[field_index][frame_i] + canvas_p.y;
		draw_list->AddCircle(ImVec2(x, y), 2.0, line_colors_xx[field_index], 5, 0.1);
	}

	auto& ctrl_x = get_ctrl_pts_x(field_index);
	auto& ctrl_y = get_ctrl_pts_y(field_index);
	for (int ctrl_i{}; ctrl_i < ctrl_x.size(); ++ctrl_i)
	{
		float x = ctrl_x[ctrl_i] + canvas_p.x;
		float y = ctrl_y[ctrl_i] + canvas_p.y;
		draw_list->AddCircleFilled(ImVec2(x, y), 5, IM_COL32(255, 255, 0, 255), 12);
	}

	// draw halfwayline
	draw_list->AddLine(ImVec2(canvas_p.x, canvas_p.y + (canvas_sz.y / 2)),
					   ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + (canvas_sz.y / 2)),
					   IM_COL32(255, 255, 255, 100));

	draw_list->PopClipRect();
}

void AnimationController::update_and_draw()
{
	auto& frames_x = window.interpolated_x[field_index];
	auto& frames_y = window.interpolated_y[field_index];
	auto& frame_positions = window.current_frame_positions;

	auto& ctrl_pts_x = window.get_ctrl_pts_x(field_index);
	auto& ctrl_pts_y = window.get_ctrl_pts_y(field_index);

	auto initialize_control_points = [&]
	{
		const float start_y = window.canvas_sz.y / 2;
		interval_width = window.canvas_sz.x / ((float)new_ctrl_pt_count - 1);

		ctrl_pts_x.clear();
		ctrl_pts_y.clear();

		for (int i{}; i < new_ctrl_pt_count; ++i)
		{ 
			ctrl_pts_x.push_back(i * interval_width);
			ctrl_pts_y.push_back(start_y);
		}
	};

	auto handle_clickables = [&]
	{
		/* hardcoded for now */
		static int char_index = 0;
		const char* charitems = "sunohara\0nagisa\0\0";
		if (ImGui::Combo("Characters", &char_index, charitems)) {
			selected_character = &GameState::scene.characters[char_index];
		}

		if (ImGui::Combo("Field", &field_index, fielditems)) {
			interp_index = window.field_interp_history[field_index];
			new_ctrl_pt_count = window.ctrl_pts_x[field_index].size();
		}

		if (ImGui::Combo("Interpolation", &interp_index, interpitems)) { 
			window.field_interp_history[field_index] = interp_index;
		}

		if (ImGui::SliderInt("Number Of Control Points", &new_ctrl_pt_count,
						 MIN_CONTROL_POINTS, MAX_CONTROL_POINTS))
		{
			initialize_control_points();
		}
	};

	auto animate_character = [&]
	{
		for (int field_i{}; field_i < NUM_OF_FIELDS; ++field_i) {
			int step = window.current_frame_positions[field_i];
			switch (field_i)
			{
			case X_POSITION:
				selected_character->set_x(local_to_game_x(window.interpolated_y[field_i][step]));
				break;
			case Y_POSITION:
				selected_character->set_y(local_to_game_y(window.interpolated_y[field_i][step]));
				break;
			case RED:
				selected_character->rgba.r = 1.0 - window.interpolated_y[field_i][step] / canvas_size_y();
				break;
			case GREEN:
				selected_character->rgba.g = 1.0 - window.interpolated_y[field_i][step] / canvas_size_y();
				break;
			case BLUE:
				selected_character->rgba.b = 1.0 - window.interpolated_y[field_i][step] / canvas_size_y();
				break;
			case ALPHA:
				selected_character->rgba.a = 1.0 - window.interpolated_y[field_i][step] / canvas_size_y();
				break;
			}
		}

		switch (interp_index)
		{
		case LINEAR:
			get_linear_drawn(window.interpolated_x[field_index], window.interpolated_y[field_index], 
							 ctrl_pts_x, ctrl_pts_y, window.canvas_sz.x);
			break;
		case STEP:
			get_step_drawn(window.interpolated_x[field_index], window.interpolated_y[field_index], 
						   ctrl_pts_x, ctrl_pts_y, window.canvas_sz.x); 
			break;
		case SPLINE:
			get_catmull_rom_drawn(window.interpolated_x[field_index], window.interpolated_y[field_index], 
								  ctrl_pts_x, ctrl_pts_y, window.canvas_sz.x);
			break;
		case SIN:
			static int amplitude = 0;
			ImGui::SliderInt("Amplitude", &amplitude, 0, window.canvas_sz.y);
			get_sin_drawn(window.interpolated_x[field_index], window.interpolated_y[field_index], 
						  ctrl_pts_x, ctrl_pts_y, window.canvas_sz.y, amplitude);
			break;
		}
	};
	
	window.pre_update();

	if (!selected_character) { selected_character = &GameState::scene.characters[0]; }

	if (ctrl_pts_x[0] < 0)
		initialize_control_points();

	process_point_dragging(ctrl_pts_x, ctrl_pts_y);

	handle_clickables();
	animate_character();
	window.draw_window();

}


void AnimationController::process_point_dragging(std::vector<float> &ctrl_pts_x, std::vector<float> &ctrl_pts_y)
{
	static bool dragging_point	= false;

	static float* drag_point_x	= &ctrl_pts_x[0];
	static float* drag_point_y	= &ctrl_pts_y[0];
	static float left_x_bound 	= ctrl_pts_x[0];
	static float right_x_bound 	= ctrl_pts_x.back();

	if (ImGui::IsMouseDragging(0) && dragging_point)
	{

		if (drag_point_x == &ctrl_pts_x[0])
		{
			*drag_point_y = mouse_pos_local_y();
		} 
		else if (drag_point_x == &ctrl_pts_x.back())
		{
			*drag_point_y = mouse_pos_local_y();
		}
		else
		{
			*drag_point_x = mouse_pos_local_x();
			*drag_point_y = mouse_pos_local_y();
		}
	}

	if (ImGui::IsMouseClicked(0) && !dragging_point)
	{
		for (int point{}, i{}; point < ctrl_pts_x.size(); ++point, ++i)
		{
			if (std::abs(mouse_pos_local_x() - ctrl_pts_x[point]) < 7 &&
				std::abs(mouse_pos_local_y() - ctrl_pts_y[point]) < 7)
			{
				dragging_point = true;

				drag_point_x = &ctrl_pts_x[point];
				drag_point_y = &ctrl_pts_y[point];
			}
		}
	}
	if (ImGui::IsMouseReleased(0))
	{
		dragging_point = false;
	}
}


float AnimationController::mouse_pos_local_x() { return window.mouse_pos_canvas.x; }
float AnimationController::mouse_pos_local_y() { return window.mouse_pos_canvas.y; }
float AnimationController::mouse_pos_global_x() { return window.mouse_pos_global.x; }
float AnimationController::mouse_pos_global_y() { return window.mouse_pos_global.y; }
float AnimationController::canvas_size_x() { return window.canvas_sz.x; };
float AnimationController::canvas_size_y() { return window.canvas_sz.y; };
float AnimationController::local_to_game_x(float value) { return ((window.canvas_sz.y - value) / window.canvas_sz.y) * GameState::w_h.x; }
float AnimationController::local_to_game_y(float value) { return ((window.canvas_sz.y - value) / window.canvas_sz.y) * GameState::w_h.y; }

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
