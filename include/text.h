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
	DialogLines(std::string dialog, float x, float y);

	void update_cursor();
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
