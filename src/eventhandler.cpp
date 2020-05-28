#include "eventhandler.h"
#include "geometry.h"
#include "parser.h"
#include "gamestate.h"
#include "eventtypes.h"

void EventHandler::process() {
	events.remove_if([](const auto& event)
	{
		return !event->on_going();
	});

	if (events.empty() && GameState::parsing)
	{
		push_back(new ParseEvent{});
	}
}

