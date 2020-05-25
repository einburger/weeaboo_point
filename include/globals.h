#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <ctime>
#include <ctype.h>
#include <vector>
#include <string>
#include <array>
#include <list>
#include <utility>

#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <filesystem>

#include <memory>
#include <utility>
#include <functional>
#include <optional>

#include "gamestate.h"
#include "eventhandler.h"
#include "geometry.h"
#include "parser.h"

extern std::unique_ptr<EventHandler> event_handler;

#define arr_size(a) sizeof(a) / sizeof(&a[0])

const std::string FONT_PATH = "../fonts/";
const std::string SCRIPT_PATH = "../scripts/";
const std::string TEXTBOX_BG_PATH = "../textbox_bg/";

enum COLOR
{
	BLACK,
	WHITE
};



