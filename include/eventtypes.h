#pragma once
#include <functional>

#include "parser.h"
#include "eventhandler.h"


struct ParseEvent : BaseEvent
{ bool on_going () override { return parse(); } };

struct MoveAnimationEvent : BaseEvent 
{
	ImVector <ImVec2> positions{};
	Character* ch{};
	int step = 0;

	MoveAnimationEvent(ImVector<ImVec2> pos, Character* ch) : positions(pos), ch(ch) {}

	bool on_going () override
	{
		const float x = ((300.0f - positions[step].x)/300.0f) * GameState::w_h[0];
		const float y = ((500.0f - positions[step].y)/500.0f) * GameState::w_h[1];
		ch->set_x(x);
		step++;
		return step < positions.size();
	}
};

struct ColorAnimationEvent : BaseEvent
{
	ImVector<ImVec4> colors{};
	Character* ch{};
	bool on_going() override {}
};

struct DialogEvent : BaseEvent
{
	DialogEvent(const std::string& dialog)
	{
		const int x = GameState::scene.textbox.min_xy[0] 
					+ GameState::scene.textbox.w_h[0] * 0.08;
		const int y = GameState::scene.textbox.min_xy[1]
			+ GameState::scene.textbox.w_h[1] * 0.3;
		GameState::scene.textfield.dl = DialogLines(dialog, x, y);
	}

	bool on_going() override
	{
		return GameState::scene.textfield.write_animation();
	}
};

struct CharacterMoveEvent : BaseEvent
{
	Character& ch;
	CharacterMoveEvent(Character& ch) : ch(ch) {}
	bool on_going() override
	{
		return ch.move(ch.target_pos, ch.center[1], ch.speed);
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

struct FadeInEvent : BaseEvent
{
	Character& ch;
	FadeInEvent(Character& ch) : ch(ch) {}
	bool on_going() override
	{
		return ch.fade(ch.fade_speed);
	}
};

struct FadeOutEvent : BaseEvent
{
	Character& ch;
	FadeOutEvent(Character& ch) : ch(ch) {}
	bool on_going() override
	{
		return ch.fade(-ch.fade_speed);
	}
};
