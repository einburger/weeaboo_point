#include "stdafx.h"
#include "parser.h"
#include "scene.h"
#include "text.h"

#define KEYWORD_TABLE \
X(at, "at", set_bg) \
X(play, "play", play_song) \
X(place, "place", place_character) \
X(exit_stage, "exit", exit_character) \
X(move, "move", move_character) \
X(set, "set", set_emotion) \
X(fadein, "fadein", fade_in) \
X(wait, "wait", scene_wait) 

#define X(a, b, c) a,
enum KEYWORD { KEYWORD_TABLE };
#undef X

#define X(a, b, c) b,
const char* keyword_name[] = { KEYWORD_TABLE };
#undef X

#define X(a, b, c) c,
void(*funct[])(int argc, char** argv) = { KEYWORD_TABLE };
#undef X

void read_file()
{
	if (!game_state->current_file)
		exit(EXIT_FAILURE);

	while (1)
	{
		Line* sline = PUSH_OBJECT(&script_pool, Line);
		if (fgets(sline->line, sizeof(sline->line), game_state->current_file))
		{
			// printf("%s", sline->line);
		}
		else
		{
			POP_OBJECT(&script_pool, Line);
			break;
		}
	}
}

Line* parser_get_line(size_t line_number)
{
	Line* xline = ((Line*)script_pool.base) + line_number;
	if (line_number * sizeof(Line) < script_pool.total_size)
	{
		return xline;
	}
	else
	{
		return NULL;
	}
}

void parse()
{
	Line* sline = parser_get_line(line_number);
	char* line = sline->line;
	if (sline)
	{
		line_number++;

		char* tkn = strtok(line, " ");
		if (tkn && strcmp(tkn, ">") == 0)
		{
			write_line(&line[2]); // don't write "> " so start at i=2
			return;
		}

		for (int i = 0; i < arr_size(keyword_name); i++)
		{
			if (tkn && strncmp(tkn, keyword_name[i], sizeof(tkn)) == 0)
			{
				char* tkns[MAX_ARGS];
				tkn = strtok(NULL, " ");

				int argc = 0;
				for (; tkn; ++argc)
				{
					tkns[argc] = tkn;

					char* nl = strchr(tkns[argc], '\n');
					if (nl) { *nl = 0; }

					tkn = strtok(NULL, " ");
				}

				funct[i](argc, tkns);

				// peak to see if this instruction syncs with next 
				Line* next_line = parser_get_line(line_number);
				if (strncmp(next_line->line, "sync", 4) == 0)
				{
					line_number++;
					parse(); // execute next if it does
				}
				return;
			}
		}
	}
}

void set_bg(int argc, char** argv)
{
	int width = game_state->window_width;
	int height = game_state->window_height;
	if (!game_state->scene->background)
	{
		game_state->scene->background = geometry_box_create(0.0, 0.0, width, height);
	}

	char path[256];
	text_append(path, BG_PATH, argv[0]);
	geometry_box_texture(game_state->scene->background, path);
}

void play_song(int argc, char** argv)
{}

void place_character(int argc, char** argv)
{
	Character* ch = scene_get_character(argv[0]);
	ch->speed = 15;
	int x_pos = (((float)atoi(argv[1]) / 100.0) * game_state->window_width) - (ch->w/2), 
		y_pos = game_state->window_height - ch->h;
	geometry_box_position(ch, x_pos, y_pos);
}

void set_emotion(int argc, char** argv)
{
	static char path[1024];
	text_append(path, CHARACTER_PATH, argv[0]);
	strcat(strcat(strcat(path, "/"), argv[1]), ".png");

	Character* ch = scene_get_character(argv[0]);
	geometry_box_texture(ch, path);
}

void exit_character(int argc, char** argv)
{
	Character* ch = scene_get_character(argv[0]);
	int target_pos = strncmp(argv[1], "left", 4) == 0 ? -ch->sprite.w : game_state->window_width;
	ch->target_pos = target_pos;
	eventhandler_event_create(MOVE, ch);
}

void move_character(int argc, char** argv)
{
	Character* ch = scene_get_character(argv[0]);
	ch->target_pos = ((float)atoi(argv[1]) / 100.0) * game_state->window_width;
	ch->speed = ((float)atoi(argv[2]));
	eventhandler_event_create(MOVE, ch);
}

void fade_in(int argc, char** argv)
{
	Character* ch = scene_get_character(argv[0]);
	ch->fade_speed = (float)atof(argv[1]);
	eventhandler_event_create(FADE, ch);
}

void scene_wait(int argc, char** argv)
{
	Character* ch = scene_get_character(argv[0]);
	ch->wait_time = atof(argv[1]);
	eventhandler_event_create(WAIT, ch);
}

void write_line(const char* line)
{
	eventhandler_event_create(WRITE, line);
	strcpy(game_state->scene->dialog, line);
}

