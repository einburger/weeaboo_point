#pragma once
#include <vector>
#include <unordered_map>

#include "imgui.h"
#include "graphing.h"


enum FIELD
{
	X_POSITION = 0,
	Y_POSITION,
	RED,
	GREEN,
	BLUE,
	ALPHA,
	NUM_OF_FIELDS
};

enum INTERP_TYPE{
	LINEAR = 0,
	STEP,
	SPLINE,
	SIN,
	NUM_OF_INTERP_TYPES
};

struct AnimationWindow
{
	static const size_t FRAME_COUNT{ 500 };
	static const size_t MAX_CONTROL_POINTS{ 12 };

	// index is field_index
	std::vector<size_t> current_frame_positions{ 0,0,0,0,0,0 };

	// field_index * num_of_fields + interp_index is the key
	std::vector<std::vector<float> > ctrl_pts_x{ NUM_OF_FIELDS, std::vector<float>(2, -10) };
	std::vector<std::vector<float> > ctrl_pts_y{ NUM_OF_FIELDS, std::vector<float>(2, 300/2) };

	// index is the field_index
	std::vector<std::vector<float> > interpolated_x{ NUM_OF_FIELDS, std::vector<float>(FRAME_COUNT, 0) };
	std::vector<std::vector<float> > interpolated_y{ NUM_OF_FIELDS, std::vector<float>(FRAME_COUNT, 300/2) };

	// std::vector<int> field_interp_history{};
	std::unordered_map<int, int> field_interp_history;

	// 2d index to 1d index. y_pos * x_count + x_pos
	std::vector<float>& get_ctrl_pts_x(size_t field_index_)
	{ return ctrl_pts_x[field_index_]; }

	std::vector<float>& get_ctrl_pts_y(size_t field_index_)
	{ return ctrl_pts_y[field_index_]; }


	void update_frame_positions()
	{
		for (auto &pos : current_frame_positions)
		{
			if (++pos >= FRAME_COUNT)
			{ pos = 0; }
		}
	}

	struct
	{
		ImVec2 canvas_p{};
		ImVec2 canvas_sz{ 500.0, 300.0 };
		ImVec2 mouse_pos_global{};
		ImVec2 mouse_pos_canvas{};
		ImDrawList* draw_list{ nullptr };
	};

	void pre_update();
	void draw_window();
};

struct Character;
struct AnimationController
{
	AnimationWindow window;
	Character* selected_character{nullptr};

	const char* interpitems = "linear\0step\0spline\0sin\00";
	const char* fielditems = "x\0y\0r\0g\0b\0a\0\0";

	static const int MAX_TIME_STEPS{ 300 };
	static const int MIN_TIME_STEPS{ -300 };
	static const int MAX_CONTROL_POINTS{ 10 };
	static const int MIN_CONTROL_POINTS{ 2 };

	float interval_width{0};
	int new_ctrl_pt_count = 2;

	void update_and_draw();

	void process_point_dragging(std::vector<float> &ctrl_pts_x, 
								std::vector<float> &ctrl_pts_y);

	float mouse_pos_local_x();
	float mouse_pos_local_y();
	float mouse_pos_global_x();
	float mouse_pos_global_y();
	float canvas_size_x();
	float canvas_size_y();
	float local_to_game_x(float value);
	float local_to_game_y(float value);
};