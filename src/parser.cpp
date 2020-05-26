#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>

#include "globals.h"
#include "parser.h"
#include "geometry.h"
#include "eventhandler.h"
#include "eventtypes.h"

std::unordered_map<std::string, void(*)(Args)> map({
		{"at", set_bg},
		{"play", play_song},
		{"place", place_character},
		{"exit", exit_character},
		{"move", move_character},
		{"set", set_emotion},
		{"fadein", fade_in},
		{"fadeout", fade_out},
		{"wait", scene_wait},
		{">", write_line}
												   });

// when traversing quickly don't generate events just directly
// set the updates
std::unordered_map<std::string, void(*)(Args)> speed_map({
		{"at", set_bg}, // already speedy
		{"play", play_song}, // skip
		{"place", place_character}, // already speedy
		{"exit", speed_exit_character},
		{"move", speed_move_character},
		{"set", set_emotion}, // already speedy
		{"fadein", speed_fade_in},
		{"fadeout", speed_fade_out},
		{"wait", scene_wait}, // skip
		{">", speed_write_line}
														 });

bool parse()
{
	if (line_number > GameState::scene.script.size()) {
		exit(EXIT_SUCCESS);
	}

	std::istringstream ss(GameState::scene.script[line_number++]);

	std::string token{};
	ss >> token; // represents the command. at, play, move, etc.

	std::vector<std::string> remaining_tokens(1); // arguments from the command
	while (ss >> remaining_tokens.back())
	{
		remaining_tokens.push_back("");
	}
	remaining_tokens.pop_back(); // last is empty;
	GameState::scene.save(GameState::scene.saves.size());

	if (const auto& itr = map.find(token); itr != map.end())
	{
		const auto& [token_key, function] = *itr;
		function(remaining_tokens);
		return false;
	}

	return true;
}

void set_bg(std::vector<std::string>& argv)
{
	const auto& bg_name{ argv[0] };
	std::string fullpath = "../backgrounds/" + bg_name + ".png";
	GameState::scene.background.set_texture(fullpath);
}


void play_song(std::vector<std::string>& argv)
{}

void place_character(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto emotion{ argv[1] };
	const auto& x_pos{ std::stof(argv[2]) };
	std::string fullpath = "../characters/" + character_name + "/" + emotion + ".png";

	auto& ch = GameState::scene.get_character(character_name);

	ch.set_texture(fullpath);
	ch.scale_to_screen(); // scale texture to size

	auto percent_screen_width = [&]()
	{
		auto character_half_width = [&]() { return ch.w_h[0] / 2; };
		return (x_pos / 100.0) * GameState::w_h[0] - character_half_width();
	};

	ch.set_pos((int)percent_screen_width(), GameState::w_h[1] - ch.w_h[1]*0.5f);
}

void set_emotion(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const auto& emotion{ argv[1] };

	auto& ch = GameState::scene.get_character(character_name);
	std::string fullpath = "../characters/" + character_name + "/" + emotion + ".png";

	ch.set_texture(fullpath); // need fullpath
}

void exit_character(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const auto& direction{ argv[1] };

	auto& ch = GameState::scene.get_character(character_name);
	int target_pos = direction == "left" ? -ch.w_h[0] : GameState::w_h[0];
	ch.target_pos = target_pos;

	event_handler->push_back(new CharacterMoveEvent(ch));
	event_handler->push_back(new FadeOutEvent(ch));

	if (argv.back() == "sync")
	{
		event_handler->push_back(new ParseEvent{});
	}
}

void speed_exit_character(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const auto& direction{ argv[1] };

	auto& ch = GameState::scene.get_character(character_name);
	int target_pos = direction == "left" ? -ch.w_h[0] : GameState::w_h[0];
	ch.target_pos = target_pos;
	ch.set_pos(target_pos, ch.min_xy[1]);
}

void move_character(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto x_pos{ std::stof(argv[1]) };
	auto speed{ std::stof(argv[2]) };

	auto& ch = GameState::scene.get_character(character_name);

	auto percent_screen_width = [&]()
	{
		auto character_half_width = [&]() { return ch.w_h[0] / 2; };
		return (x_pos / 100.0) * GameState::w_h[0]
			- character_half_width();
	};

	ch.target_pos = percent_screen_width();
	ch.speed = speed;

	event_handler->push_back(new CharacterMoveEvent(ch));

	if (argv.back() == "sync")
	{
		event_handler->push_back(new ParseEvent{});
	}
}

void speed_move_character(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto x_pos{ std::stof(argv[1]) };
	auto speed{ std::stof(argv[2]) };

	auto& ch = GameState::scene.get_character(character_name);

	auto percent_screen_width = [&]()
	{
		auto character_half_width = [&]() { return ch.w_h[0] / 2; };
		return (x_pos / 100.0) * GameState::w_h[0]
			- character_half_width();
	};

	ch.target_pos = percent_screen_width();
	ch.set_pos(ch.target_pos, ch.min_xy[1]);
	ch.speed = speed;
}

void fade_in(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto speed{ std::stof(argv[1]) };

	auto& ch = GameState::scene.get_character(argv[0]);
	ch.fade_speed = speed;

	event_handler->push_back(new FadeInEvent(ch));

	if (argv.back() == "sync")
	{
		event_handler->push_back(new ParseEvent());
	}
}

void speed_fade_in(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto speed{ std::stof(argv[1]) };

	auto& ch = GameState::scene.get_character(argv[0]);
	ch.fade_speed = speed;
	ch.rgba[3] = 1.0f;
}

void fade_out(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const float speed{ std::stof(argv[1]) };

	auto& ch = GameState::scene.get_character(character_name);
	ch.fade_speed = speed;

	event_handler->push_back(new FadeOutEvent(ch));

	if (argv.back() == "sync")
	{
		event_handler->push_back(new ParseEvent());
	}
}

void speed_fade_out(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const float speed{ std::stof(argv[1]) };

	auto& ch = GameState::scene.get_character(character_name);
	ch.fade_speed = speed;
	ch.rgba[3] = 0.0f;
}

void scene_wait(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const float wait_time{ std::stof(argv[1]) };

	auto& ch = GameState::scene.get_character(argv[0]);
	ch.wait_time = wait_time;
}

void write_line(std::vector<std::string>& argv)
{
	auto merge_tokens = [&]()
	{
		std::string line{};
		for (const auto& word : argv)
		{
			line += word + " ";
		}
		return line;
	};

	GameState::scene.dialog = merge_tokens();
	event_handler->push_back(new DialogEvent(GameState::scene.dialog));
}

void speed_write_line(std::vector<std::string>& argv)
{
	auto merge_tokens = [&]()
	{
		std::string line{};
		for (const auto& word : argv)
		{
			line += word + " ";
		}
		return line;
	};
	GameState::scene.dialog = merge_tokens();
	GameState::text_cursor_pos = GameState::scene.dialog.size();
}


