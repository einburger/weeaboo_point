#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>

#include "globals.h"
#include "parser.h"
#include "geometry.h"
#include "eventhandler.h"
#include "eventtypes.h"
#include "graphing.h"

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
	GameState::scene.dialog = "";
	if (line_number >= GameState::scene.script.size()) {
		// we've parsed all the lines in the script, parsing is done
		GameState::parsing = false;
		return false;
	}
	std::istringstream ss(GameState::scene.script[line_number]);

	// represents the command. at, play, move, etc.
	std::string token{};
	ss >> token; 

	// arguments passed to the command
	std::vector<std::string> remaining_tokens(1); 

	// remaining tokens looks like this { "4", "2", "" }  where "" is a placeholder for the next arg
	// to be parsed
	while (ss >> remaining_tokens.back())
	{
		// we dont' know how many args there are, so push back an empty string as a placeholder 
		remaining_tokens.push_back("");
	}
	// last is empty because we added empty strings anticipating another arg (removing the placeholder)
	remaining_tokens.pop_back(); 

	// find the function associated with the parsed action name
	if (const auto& itr = map.find(token); itr != map.end())
	{
		const auto& [token_key, function] = *itr;

		// call the function 
		function(remaining_tokens);
		
		GameState::scene.save(GameState::scene.saves.size());

		// each line has a sequence of commands associated with it (character A moves, character B talks, ...)
		// store these actions in a vector of vectors of command_histories so we can re-play any line
		GameState::scene.command_history[line_number].push_back(
			[&function, remaining_tokens]{ function(remaining_tokens); }
		);

		line_number++;
		return false;
	}

	GameState::scene.save(GameState::scene.saves.size());
	line_number++;
	return true;
}

void set_bg(std::vector<std::string> argv)
{
	const auto& bg_name{ argv[0] };
	std::string fullpath = "../backgrounds/" + bg_name + ".png";
	GameState::scene.background.set_texture(fullpath);
}


void play_song(std::vector<std::string> argv)
{}

void place_character(std::vector<std::string> argv)
{
	const auto& character_name{ argv[0] };
	auto emotion{ argv[1] };
	const auto& x_pos{ std::stof(argv[2]) };
	std::string fullpath = "../characters/" + character_name + "/" + emotion + ".png";

	auto& ch = GameState::scene.get_character(character_name);

	ch.set_texture(fullpath);
	ch.scale_to_screen(); // scale texture to size
	ch.set_pos((x_pos/100) * GameState::w_h.x , GameState::w_h.y - (ch.w_h.y * 0.5f));
}

void set_emotion(std::vector<std::string> argv)
{
	const auto& character_name{ argv[0] };
	const auto& emotion{ argv[1] };

	auto& ch = GameState::scene.get_character(character_name);
	std::string fullpath = "../characters/" + character_name + "/" + emotion + ".png";

	ch.set_texture(fullpath); // need fullpath
}

void move_character(std::vector<std::string> argv)
{
	const auto& character_name{ argv[0] };
	const auto& interp_method{ argv[1] };
	int time_steps{ std::stoi(argv[2]) };

	auto& ch = GameState::scene.get_character(character_name);

	ImVector<ImVec2> points;
	for (size_t i{ 3 }; i < argv.size(); i += 2)
	{
		points.push_back( ImVec2(std::stoi(argv[i]), std::stoi(argv[i + 1])) );
	}

	if (interp_method == "linear") 
		event_handler->push_back(new MoveEvent(&ch, get_linear(points[0], points[1], time_steps)));
	if (interp_method == "spline")
		event_handler->push_back(new MoveEvent(&ch, get_catmull_rom(points, time_steps)));
}

void fade(std::vector<std::string> argv)
{
	const auto& character_name{ argv[0] };
	auto &ch = GameState::scene.get_character(character_name);
	const auto& interp_method{ argv[1] };
	int time_steps{ std::stoi(argv[2]) };

	ImVector<ImVec2> points;
	for (size_t i{ 3 }; i < argv.size(); i += 2)
	{
		points.push_back(ImVec2(std::stof(argv[i]), std::stof(argv[i + 1])));
	}

	if (interp_method == "linear") 
		event_handler->push_back(new ColorEvent(&ch, get_linear(points[0], points[1], time_steps)));
	if (interp_method == "spline") 
		event_handler->push_back(new ColorEvent(&ch, get_catmull_rom(points, time_steps)));
}

void write_line(std::vector<std::string> argv)
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

void scene_wait(std::vector<std::string> argv) { }
