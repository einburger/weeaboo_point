#include "stdafx.h"
#include "globals.h"
#include "eventhandler.h"
#include "parser.h"
#include "scene.h"
#include "text.h"
#include "geometry.h"
#include "memorypool.h"

Event* eventhandler_event_create(int ID, void* obj)
{
	Event* event = PUSH_OBJECT(&event_stack, Event);
	event->ID = ID;
	event->obj = obj;
	return event;
}

void eventhandler_event_handle()
{
	Event* event = NULL; // no events to handle, so read next line
	if (event_stack.used_size == 0) { event = eventhandler_event_create(PARSE, NULL); }

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
			if (!parse())
				move_to_end_and_remove(event);
			break;
		case WRITE:
			if (!text_write_animation((const char*)event->obj))
			{ // wait for input after line is written
				move_to_end_and_remove(event);
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
