#pragma once
#include <functional>

#include "parser.h"
#include "eventhandler.h"

struct ParseEvent : BaseEvent
{ bool on_going () override { return parse(); } };

struct MoveEvent : BaseEvent 
{
	Character* ch;
	ImVector <ImVec2> positions{};
	size_t step = 0;

	MoveEvent(Character* ch, ImVector<ImVec2> vec) : ch(ch), positions(vec) {}

	bool on_going() override
	{
		const float x = ((300.0f - positions[step].y) / 300.0f) * GameState::w_h.x;
		const float y = ((500.0f - positions[step].y) / 500.0f) * GameState::w_h.y;
		ch->set_x(x);
		return ++step < positions.size();
	}
};

struct ColorEvent : BaseEvent
{
	Character* ch;
	ImVector<ImVec4> colors;
	size_t step = 0;

	ColorEvent(Character* ch, ImVector<ImVec4> cols) : ch(ch), colors(cols) {}

	bool on_going() override
	{
		ch->set_color(colors[step].x, colors[step].y, colors[step].z, colors[step].w);
		return ++step < colors.size();
	}
};

struct DialogEvent : BaseEvent
{
	DialogEvent(const std::string& dialog)
	{
		const int x = GameState::scene.textbox.min_xy.x 
					+ GameState::scene.textbox.w_h.x * 0.08;
		const int y = GameState::scene.textbox.min_xy.y
			+ GameState::scene.textbox.w_h.y * 0.3;
		GameState::scene.textfield.dl = DialogLines(dialog, x, y);
	}

	bool on_going() override
	{
		return GameState::scene.textfield.write_animation();
	}
};

struct WaitEvent : BaseEvent
{
	WaitEvent() = default;
	bool on_going() override
	{
		return false;
	}
};

