#pragma once
#include <GLFW/glfw3.h>

#define arr_size(a) sizeof(a) / sizeof(&a[0])

const std::string FONT_PATH = "../fonts/";
const std::string SCRIPT_PATH = "../scripts/";
const std::string TEXTBOX_BG_PATH = "../textbox_bg/";

enum COLOR
{
	BLACK,
	WHITE
};

