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
			if (!geometry_box_move(ch, ch->target_pos, ch->y_min, ch->speed))
				move_to_end_and_remove(event);
			break;
		case FADE:
			if (!geometry_box_fade(ch, ch->fade_speed))
				move_to_end_and_remove(event);
			break;
		case WAIT:
			if (!geometry_box_wait(ch->wait_time))
				move_to_end_and_remove(event);
			break;
		case PARSE:
			POP_OBJECT(&event_stack, Event);
			parse();
			break;
		case WRITE:
			if (game_state->text_cursor_pos < strlen((const char*)event->obj))
			{
				game_state->text_cursor_pos++;
				text_draw((const char*)event->obj);
			}
			else
			{ // line was written, so wait for input after line is written
				POP_OBJECT(&event_stack, Event);
				eventhandler_event_create(GET_INPUT, NULL);
			}
			break;
		case GET_INPUT:
			scene_get_input();
			break;
		}
	}
}

void move_to_end_and_remove(Event* e)
{
	Event* last = GET_END(&event_stack, Event);
	e->ID = last->ID;
	e->obj = last->obj;
	POP_OBJECT(&event_stack, Event);
}
