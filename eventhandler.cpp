#include "stdafx.h"
#include "globals.h"
#include "eventhandler.h"
#include "parser.h"
#include "scene.h"
#include "text.h"
#include "geometry.h"
#include "memorypool.h"

void eventhandler_event_create(int ID, void *obj) 
{
	Event *event = PUSH_OBJECT(&event_stack, Event);
	event->ID = ID;
	event->obj = obj;

	switch (ID)
	{
	case MOVE:
		event_move = geometry_box_move;
		break;
	case WAIT:
		event_wait = geometry_box_wait;
		break;
	case FADE:
		event_fade = geometry_box_fade;
		break;
	case PARSE:
		event_parse = parse;
		break;
	case WRITE:
		event_write = text_draw;
		break;
	case GET_INPUT:
		event_input = scene_get_input;
	default:
		break;
	}
}


void eventhandler_event_handle()
{
	Event *event = NULL;
	if (event_stack.used_size == 0) {
		eventhandler_event_create(PARSE, NULL);
		event = GET_END(&event_stack, Event);
	}

	for (size_t i = 0; i * sizeof(Event) < event_stack.used_size; ++i) {
		event = ((Event *)event_stack.base) + i;

		Character* ch = (Character *)event->obj;
		switch (event->ID) {
		case MOVE:
			if (!event_move(ch, ch->target_pos, ch->y_min, ch->speed)) {
				move_to_end_and_remove(event);
			}
			break;
		case FADE:
			if (!event_fade(ch, ch->speed)) { 
				move_to_end_and_remove(event);
			}
			break;
		case WAIT:
			if (!event_wait(ch->wait_time)) {
				move_to_end_and_remove(event);
			}
			break;
		case PARSE:
			POP_OBJECT(&event_stack, Event);
			event_parse();
			break;
		case WRITE:
			if (game_state->text_cursor_pos < strlen((const char*)event->obj)) {
				game_state->text_cursor_pos++;
				event_write((const char*)event->obj);
			} else { // line was written, so wait for input after line is written
				POP_OBJECT(&event_stack, Event);
				eventhandler_event_create(GET_INPUT, NULL); 
			}
			break;
		case GET_INPUT:
			event_input();
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
