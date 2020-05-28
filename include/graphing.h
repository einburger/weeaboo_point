#pragma once
#include "imgui.h"

ImVector<ImVec2> get_bez(ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3, int time_slices);
// ImVector<ImVec2> get_catmull_rom(ImVector<ImVec2>& points, int time_slices);

template <typename T = ImVec2>
ImVector<T> get_catmull_rom_drawn(ImVector<ImVec2> &points, int time_slices)
{
	if (points.Size < 4) {
		return {};
	}

	ImVector<ImVec2> vec;
	ImVec2 point;
	vec.push_back(points[1]);

	auto catmull_rom = [](float a, float b, float c, float d, float t)
	{
		return 0.5f * ((-a + 3 * b - 3 * c + d) * t * t * t
					  + (2 * a - 5 * b + 4 * c - d) * t * t
					  + (-a + c) * t
					  + 2 * b);
	};

	auto uniform_catmull_rom = [&](ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3)
	{
		double dx = 0.005f;
		double prev_t = 0.0f;
		const double target_delta = 500.0f / time_slices;
		while (prev_t < 1.0f)
		{
			float temp_x_curr = catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t);
			do
			{
				prev_t += dx;
			} while (std::abs(catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t)) - temp_x_curr < target_delta);

			point.x = (catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t));
			point.y = (catmull_rom(p0.y, p1.y, p2.y, p3.y, prev_t));
			vec.push_back(point);
		}
		vec.pop_back();
	};

	for (int i{ 3 }; i < points.Size; ++i)
	{
		uniform_catmull_rom(points[i-3], points[i-2], points[i-1], points[i]);
	}
	
	vec.push_back(points[points.Size - 2]);
	return vec;
}

template <typename T = float>
ImVector<T> get_catmull_rom(ImVector<ImVec2> &points, int time_slices)
{
	if (points.Size < 4) {
		return {};
	}

	ImVector<float> vec;
	vec.push_back(points[1].y);

	auto catmull_rom = [](float a, float b, float c, float d, float t)
	{
		return 0.5f * ((-a + 3 * b - 3 * c + d) * t * t * t
					  + (2 * a - 5 * b + 4 * c - d) * t * t
					  + (-a + c) * t
					  + 2 * b);
	};

	auto uniform_catmull_rom = [&](ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3)
	{
		double dx = 0.005f;
		double prev_t = 0.0f;
		const double target_delta = 500.0f / time_slices;
		while (prev_t < 1.0f)
		{
			float temp_x_curr = catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t);
			do
			{
				prev_t += dx;
			} while (std::abs(catmull_rom(p0.x, p1.x, p2.x, p3.x, prev_t)) - temp_x_curr < target_delta);

			vec.push_back(catmull_rom(p0[1], p1[1], p2[1], p3[1], prev_t));
		}
		// vec.pop_back();
	};

	for (int i{ 3 }; i < points.Size; ++i)
	{
		uniform_catmull_rom(points[i-3], points[i-2], points[i-1], points[i]);
	}
	
	vec.push_back(points[points.Size - 2].y);
	return vec;
}


template <typename T = float>
ImVector<T> get_linear(ImVec2 start, ImVec2 end, int time_slices)
{
	auto linear_interp= [](float start, float end, float t)
	{
		return ((1.0f - t) * start) + (t * end);
	};

	ImVector<float> points;
	for (int i{}; i < time_slices; ++i)
	{
		float t = ((float)i) / ((float)time_slices);
		points.push_back(linear_interp(start.x, end.x, t));
	}

	return points;
}


// ImVector<ImVec2> get_sin(ImVec2 &start, int time_slices);
// ImVector<ImVec2> get_step(ImVector <ImVec2>& points, int time_slices);
