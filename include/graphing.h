#pragma once
#include "imgui.h"

ImVector<ImVec2> get_bez(ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3, int time_slices);

template <typename T = ImVec2>
ImVector<T> get_sin_drawn(ImVector<ImVec2>& vec, int canvas_x, int amplitude)
{
	ImVector<ImVec2> points;
	int idx{};
	for (int k{ 2 }; k < vec.size()-1; ++k)
	{
		for (int i{}; i <= std::abs(vec[k].x - vec[k - 1].x); ++i)
		{
			float t = (float)i / (float)std::abs(vec[k].x - vec[k - 1].x);
			float y = std::cos(t*(22/7)) * (amplitude/2) * (k%2==0?-1:1) + (canvas_x/2);
			points.push_back(ImVec2(idx++, y));
		}
	}

	return points;
}

template <typename T = ImVec2>
ImVector<T> get_step_drawn(ImVector<ImVec2>& vec, int canvas_x)
{
	ImVector<ImVec2> points;
	int k{};
	for (int i{ 2 }; i < vec.size()-1; i++)
	{
		for (int j{}; j < std::abs(vec[i].x - vec[i-1].x); ++j)
		{
			points.push_back(ImVec2(k++, vec[i-1].y));
		}
	}
	return points;
}

template <typename T = ImVec2>
ImVector<T> get_catmull_rom_drawn(ImVector<ImVec2> &vec, int canvas_x)
{
	if (vec.Size < 4) {
		return {};
	}

	ImVector<ImVec2> points;
	ImVec2 point;
	//points.push_back(vec[1]);

	auto catmull_rom = [](float a, float b, float c, float d, float t)
	{
		return 0.5f * ((-a + 3 * b - 3 * c + d) * t * t * t
					  + (2 * a - 5 * b + 4 * c - d) * t * t
					  + (-a + c) * t
					  + 2 * b);
	};

	auto uniform_catmull_rom = [&](ImVec2& p0, ImVec2& p1, ImVec2& p2, ImVec2& p3)
	{
		for (int i{}; i < canvas_x; ++i)
		{
			float t = (float)i / (float)canvas_x;

			int x = catmull_rom(p0.x, p1.x, p2.x, p3.x, t);
			float y = catmull_rom(p0.y, p1.y, p2.y, p3.y, t);

			bool found = false;
			for (auto& p : points)
			{
				if (x < 0 || x >= canvas_x) 
					break;
				if ((int)p.x == x)
				{
					found = true;
					p.y = y;
					break;
				}
			}

			if (!found && (x>0&&x<canvas_x)) { points.push_back(ImVec2(x, y)); }
		}

	};

	for (int i{ 3 }; i < vec.Size; ++i)
	{
		uniform_catmull_rom(vec[i-3], vec[i-2], vec[i-1], vec[i]);
	}
	
	//points.push_back(vec[vec.Size - 2]);
	return points;
}
template <typename T = float>
ImVector<ImVec2> get_linear_drawn(ImVector<ImVec2>& vec, int canvas_x)
{
	auto linear_interp= [](float start, float end, float t)
	{
		return ((1.0f - t) * start) + (t * end);
	};

	ImVector<ImVec2> points;
	int idx{};
	for (int k{ 2 }; k < vec.size()-1; ++k)
	{
		for (int i{}; i < std::abs(vec[k].x - vec[k-1].x); ++i)
		{
			float t = (float)i / (float)std::abs(vec[k].x-vec[k-1].x);
			float y = linear_interp(vec[k - 1].y, vec[k].y, t);
			points.push_back(ImVec2(idx++, y));
		}
	}

	return points;
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


// ImVector<ImVec2> get_sin(ImVec2 &start, int time_slices);
// ImVector<ImVec2> get_step(ImVector <ImVec2>& points, int time_slices);
