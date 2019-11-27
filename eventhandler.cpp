#include "stdafx.h"
#include "globals.h"
#include "eventhandler.h"
#include "parser.h"
#include "scene.h"
#include "text.h"
#include "geometry.h"
#include "memorypool.h"

void eventhandler_event_create(int ID, void* obj)
{
	Event* event = PUSH_OBJECT(&event_stack, Event);
	event->ID = ID;
	event->obj = obj;

	switch (ID)
	{
	case MOVE:
		EVENT_MOVE = geometry_box_move;
		break;
	case WAIT:
		EVENT_WAIT = geometry_box_wait;
		break;
	case FADE:
		EVENT_FADE = geometry_box_fade;
		break;
	case PARSE:
		EVENT_PARSE = parse;
		break;
	case WRITE:
		EVENT_WRITE = text_draw;
		break;
	case GET_INPUT:
		EVENT_INPUT = scene_get_input;
	default:
		break;
	}
}


void eventhandler_event_handle()
{
	Event* event = NULL;
	if (event_stack.used_size == 0)
	{ // process the next line if no events to handle
		eventhandler_event_create(PARSE, NULL);
		event = GET_END(&event_stack, Event);
	}

	for (size_t i = 0; i * sizeof(Event) < event_stack.used_size; ++i)
	{
		event = ((Event*)event_stack.base) + i;

		Character* ch = (Character*)event->obj;
		switch (event->ID)
		{
		case MOVE:
			if (!EVENT_MOVE(ch, ch->target_pos, ch->y_min, ch->speed))
				move_to_end_and_remove(event);
			break;
		case FADE:
			if (!EVENT_FADE(ch, ch->fade_speed))
				move_to_end_and_remove(event);
			break;
		case WAIT:
			if (!EVENT_WAIT(ch->wait_time))
				move_to_end_and_remove(event);
			break;
		case PARSE:
			POP_OBJECT(&event_stack, Event);
			EVENT_PARSE();
			break;
		case WRITE:
			if (game_state->text_cursor_pos < strlen((const char*)event->obj))
			{
				game_state->text_cursor_pos++;
				EVENT_WRITE((const char*)event->obj);
			}
			else
			{ // line was written, so wait for input after line is written
				POP_OBJECT(&event_stack, Event);
				eventhandler_event_create(GET_INPUT, NULL);
			}
			break;
		case GET_INPUT:
			EVENT_INPUT();
			break;
		}
	}
}

void eventhandler_event_a_eq_b(Event* a, Event* b)
{
	a->ID = b->ID;
	a->obj = b->obj;
	a->u = b->u;
}

void move_to_end_and_remove(Event* e)
{
	Event* last = GET_END(&event_stack, Event);
	eventhandler_event_a_eq_b(e, last);
	POP_OBJECT(&event_stack, Event);
}
