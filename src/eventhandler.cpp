#include "eventhandler.h"
#include "geometry.h"
#include "parser.h"
#include "gamestate.h"
#include "eventtypes.h"

bool printsomething()
{
	std::cout << "removing event\n";
	return true;
}

void EventHandler::process() {
	events.remove_if([](const auto& event)
	{
		return !event->on_going() && printsomething();
	});

	if (events.empty() && GameState::parsing)
	{
		push_back(new ParseEvent{});
	}
}

