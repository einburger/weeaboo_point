#pragma once
#include "imgui.h"
#include "graphing.h"

struct AnimationWindow
{

	AnimationWindow() = default;
	~AnimationWindow() = default;

	ImVec2 canvas_p{};
	ImVec2 canvas_sz{};
	ImVec2 mouse_pos_global{};
	ImVec2 mouse_pos_canvas{};
	int xy_screen_scale_x{ 0 };
	int xy_screen_scale_y{ 0 };

	ImDrawList* draw_list{ nullptr };
	ImVector<ImVec2> animation_line{};
	ImVector<ImVec2> control_points{};


	void update_and_draw();
};

struct Character;
struct AnimationController
{

	AnimationController() = default;
	~AnimationController() = default;

	AnimationWindow window{};
	Character* selected_character{};

	const int MAX_TIME_STEPS{ 300 };
	const int MIN_TIME_STEPS{ -300 };
	const int MAX_CONTROL_POINTS{ 10 };
	const int MIN_CONTROL_POINTS{ 2 };
	int number_of_control_points{ 4 };
	int time_steps{ 300 };
	int current_step{ 0 };
	float step_size{ 10.0 };

	void update_and_draw();

	float mouse_pos_local_x();
	float mouse_pos_local_y();
	float mouse_pos_global_x();
	float mouse_pos_global_y();
	float canvas_size_x();
	float canvas_size_y();
	float local_to_game_x(float value);
	float local_to_game_y(float value);
};