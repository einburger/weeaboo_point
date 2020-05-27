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
		{"move", move_character},
		{"set", set_emotion},
		{"fadein", fade},
		{"fadeout", fade},
		{"wait", scene_wait},
		{">", write_line}
												   });

bool parse()
{
	if (line_number > GameState::scene.script.size()) {
		GameState::parsing = false;
		return false;
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
	GameState::scene.save(GameState::scene.saves.size());
	GameState::scene.fns.push_back([] {});
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
	ch.set_pos(GameState::w_h.x * 0.5f, GameState::w_h.y - (ch.w_h.y * 0.5f));

	GameState::scene.fns.push_back([] {});
	GameState::scene.save(GameState::scene.saves.size());
}

void set_emotion(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const auto& emotion{ argv[1] };

	auto& ch = GameState::scene.get_character(character_name);
	std::string fullpath = "../characters/" + character_name + "/" + emotion + ".png";

	ch.set_texture(fullpath); // need fullpath

	GameState::scene.fns.push_back([] {});
	GameState::scene.save(GameState::scene.saves.size());
}

void move_character(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto x_pos{ std::stof(argv[1]) };

	ImVector <ImVec2> positions;

	for (int i{ 2 }; i < argv.size(); ++i)
	{

	}

	auto& ch = GameState::scene.get_character(character_name);

	auto percent_screen_width = [&]()
	{
		auto character_half_width = [&]() { return ch.w_h.x / 2; };
		return (x_pos / 100.0) * GameState::w_h.x - character_half_width();
	};

	event_handler->push_back(new MoveEvent(&ch, {}));

	GameState::scene.fns.push_back(
		[&]() { event_handler->push_back(new MoveEvent(&ch, {})); }
	);

	GameState::scene.save(GameState::scene.saves.size());
}

void fade(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	auto &ch = GameState::scene.get_character(argv[0]);

	ImVector<ImVec4> colors;
	for (int i{ 1 }; i < argv.size(); ++i) {
		ImVec4 color(1.0f, 1.0f, 1.0f, std::stof(argv[i]));
		colors.push_back(color);
	}

	event_handler->push_back(new ColorEvent(&ch, colors));

	GameState::scene.fns.push_back(
		[&ch, colors] { event_handler->push_back(new ColorEvent(&ch, colors)); }
	);

	GameState::scene.save(GameState::scene.saves.size());
}

void scene_wait(std::vector<std::string>& argv)
{
	const auto& character_name{ argv[0] };
	const float wait_time{ std::stof(argv[1]) };

	auto& ch = GameState::scene.get_character(argv[0]);
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
	GameState::scene.fns.push_back(
		[]{ event_handler->push_back(new DialogEvent(GameState::scene.dialog)); }
	);
	GameState::scene.save(GameState::scene.saves.size());
}

