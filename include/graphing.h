#pragma once
#include "imgui.h"

ImVector<ImVec2> get_bez(ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3, int time_slices);
ImVector<ImVec2> get_catmull_rom(ImVector<ImVec2>& points, int time_slices);
