#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <numeric>

#include "parser.h"
#include "globals.h"
#include "geometry.h"
#include "scene.h"


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

bool parse() {
    if (line_number > game_state->scene.script.size()) {
        exit(EXIT_SUCCESS);		
    }
    scene_states->update_state();

    std::istringstream ss(game_state->scene.script[line_number++]);

    std::string token{};
    ss >> token; // represents the command. at, play, move, etc.

    std::vector<std::string> remaining_tokens(1); // arguments from the command
    while (ss >> remaining_tokens.back()) 
    { remaining_tokens.push_back(""); }
    remaining_tokens.pop_back(); // last is empty;

    if (const auto &itr = map.find(token); itr != map.end()) {
        const auto &[token_key, function] = *itr;
        function(remaining_tokens);
        return false;
    }

    return true;
}

void set_bg(std::vector<std::string> &argv)
{
    decltype(auto) bg_name{argv[0]};
    const int width{game_state->window_width};
    const int height{game_state->window_height};

    game_state->scene.background = Box(0.0, 0.0, width, height);
    game_state->scene.background.set_texture(BG_PATH + bg_name);
}

void play_song(std::vector<std::string> &argv)
{}

void place_character(std::vector<std::string> &argv)
{
    decltype(auto) character_name{argv[0]};
    decltype(auto) emotion{argv[1]};
    decltype(auto) x_pos{std::stof(argv[2])};

    auto &ch = game_state->scene.get_character(character_name);
    ch.set_emotion(emotion);
    ch.scale(); // scale texture to size

    auto percent_screen_width = [&]() {
        auto character_half_width = [&]() { return ch.w_h[0]/2; };
        return (x_pos / 100.0)  * game_state->window_width - character_half_width();
    };

    auto y_moved_down = [&]() { return game_state->window_height - ch.w_h[1]; };
    ch.set_position({ (int)percent_screen_width(), y_moved_down() });

    if (argv.back() == "sync") {
        event_handler->push_back(new Event<ParseEvent>);
    }
}

void set_emotion(std::vector<std::string> &argv)
{
    const std::string& character_name{argv[0]};
    const std::string& emotion{argv[1]};

    auto& ch = game_state->scene.get_character(character_name);
    ch.set_emotion(emotion);
}

void exit_character(std::vector<std::string> &argv)
{
    const std::string& character_name{argv[0]};
    const std::string& direction{argv[1]};

    auto& ch = game_state->scene.get_character(character_name);
    int target_pos = direction == "left" ? -ch.w_h[0] : game_state->window_width;
    ch.target_pos = target_pos;

    event_handler->push_back(new Event<CharacterMoveEvent>(ch));
    event_handler->push_back(new Event<FadeOutEvent>(ch));

    if (argv.back() == "sync") {
        event_handler->push_back(new Event<ParseEvent>);
    }
}

void move_character(std::vector<std::string> &argv)
{
    decltype(auto) character_name{std::move(argv[0])};
    decltype(auto) x_pos{std::stof(argv[1])};
    decltype(auto) speed{std::stof(argv[2])};

    auto& ch = game_state->scene.get_character(character_name);

    auto percent_screen_width = [&]() {
        auto character_half_width = [&]() { return ch.w_h[0] / 2; };
        return (x_pos / 100.0)  * game_state->window_width 
            - character_half_width();
    };

    ch.target_pos = percent_screen_width();
    ch.speed = speed;

    event_handler->push_back(new Event<CharacterMoveEvent>(ch));

    if (argv.back() == "sync") {
        event_handler->push_back(new Event<ParseEvent>);
    }
}

void fade_in(std::vector<std::string> &argv)
{
    decltype(auto) character_name{argv[0]};
    decltype(auto) speed{std::stof(argv[1])};

    auto& ch = game_state->scene.get_character(argv[0]);
    ch.fade_speed = speed;

    event_handler->push_back(new Event<FadeInEvent>(ch));

    if (argv.back() == "sync") {
        event_handler->push_back(new Event<ParseEvent>);
    }
}

void fade_out(std::vector<std::string> &argv)
{
    decltype(auto) character_name{argv[0]};
    const float speed{std::stof(argv[1])};

    auto& ch = game_state->scene.get_character(character_name);
    ch.fade_speed = speed;

    event_handler->push_back(new Event<FadeOutEvent>(ch));

    if (argv.back() == "sync") {
        event_handler->push_back(new Event<ParseEvent>);
    }
}

void scene_wait(std::vector<std::string> &argv)
{
    decltype(auto) character_name{argv[0]};
    const float wait_time{std::stof(argv[1])};

    auto& ch = game_state->scene.get_character(argv[0]);
    ch.wait_time = wait_time;
}

void write_line(std::vector<std::string> &argv)
{
    auto merge_tokens = [&]() {
        std::string line{};
        for (const auto &word : argv) {
            line += word + " ";
        }
        return line;
    };

    game_state->scene.dialog = merge_tokens(); 
    event_handler->push_back(new Event<DialogEvent>(game_state->scene.dialog));
}


