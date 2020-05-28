#pragma once
#include <functional>

#include "parser.h"
#include "eventhandler.h"

struct ParseEvent : BaseEvent
{ bool on_going () override { return parse(); } };

struct MoveEvent : BaseEvent 
{
	Character* ch;
	ImVector <float> positions;
	size_t step = 0;

	MoveEvent(Character* ch, ImVector<float> vec) : ch(ch), positions(vec) {}

	bool on_going() override
	{
		ch->set_pos(positions[step], ch->center.y);
		return ++step < positions.size();
	}
};

struct ColorEvent : BaseEvent
{
	Character* ch;
	ImVector<float> colors;
	size_t step = 0;

	ColorEvent(Character* ch, ImVector<float> cols) : ch(ch), colors(cols) { }

	bool on_going() override
	{
		ch->set_alpha(colors[step]);
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

