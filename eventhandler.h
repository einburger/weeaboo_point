#pragma once
#include "geometry.h"

typedef struct Event
{
	int ID;
	void* obj;
} Event;

void eventhandler_event_create(int ID, void* obj);
void eventhandler_event_handle();

/* Once an even is handled, swap the end of the stack with it,
 * and decrement stack pointer, effectively removeing it */
static void move_to_end_and_remove(Event*);

