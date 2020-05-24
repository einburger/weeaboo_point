#pragma once
#include <string>
#include <iostream>
#include <sstream>


struct Line 
{
	std::string line{""};
	float y_pos{}, x_pos{};
	int local_cursor_pos{0};
	
	Line() = default;
	Line(float xpos, float ypos) : x_pos(xpos), y_pos(ypos) {}
	Line(std::string& dialog_line) : line(dialog_line) {}

	void draw();
};

struct DialogLines
{
	float start_x{50}, start_y{50};
	std::vector<Line> lines;

	DialogLines() = default;
	DialogLines(std::string dialog, float x, float y)
	{
		start_x = x, start_y = y;
		float ypos{ start_y };

		lines.push_back(Line(start_x, start_y));

		std::string current_word{};
		std::istringstream ss(dialog);

		int count{};
		while (ss >> current_word)
		{
			count += current_word.size() + 1;
			if (count < 50)
			{ // add current word as it's within line limit
				lines.back().line += current_word + " ";
			}
			else
			{ // adding the current word exceeds line limit
				lines.push_back(Line(start_x, ypos += 28.0f));
				lines.back().line += current_word + " ";
				count = current_word.size(); // next line count starts with current word size
			}
		}
	}

	void draw();
};

struct Field
{
	DialogLines dl;

	float font_size{ 12 };

	Field() = default;
	Field(const std::string& font, float fontsize, float xstart, float ystart)
	{
		init(font.c_str(), fontsize);
		font_size = fontsize;
	}

	void init(const char* text_path, float font_size);
	bool write_animation();
};
