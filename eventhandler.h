#pragma once
#include "geometry.h"

typedef struct Event
{
	int ID;
	void* obj;
	union
	{
		char(*move_ptr)(Box*, int, int, int);
		char(*fade_ptr)(Box*, float);
		char(*wait_ptr)(double);
		void(*write_ptr)(const char*);
		void(*parse_ptr)();
		void(*input_ptr)();
	} u;
} Event;

#define EVENT_MOVE event->u.move_ptr
#define EVENT_FADE event->u.fade_ptr
#define EVENT_WAIT event->u.wait_ptr
#define EVENT_WRITE event->u.write_ptr
#define EVENT_PARSE event->u.parse_ptr
#define EVENT_INPUT event->u.input_ptr

void eventhandler_event_create(int ID, void* obj);
void eventhandler_event_handle();
void eventhandler_event_a_eq_b(Event* a, Event* b);

/* set one event to the other */
static void eventhandler_event_a_eq_b(Event* a, Event* b);

/* Once an even is handled, swap the end of the stack with it,
 * and decrement stack pointer, effectively removeing it */
static void move_to_end_and_remove(Event*);

// void   eventhandler_event_handle(Event *event);
