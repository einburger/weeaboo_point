#pragma once
#include "imgui.h"

#include <iostream>
#include <vector>
#include <cmath>

template<class T=int>
void get_sin_drawn(std::vector<float>& frames_x, std::vector<float>& frames_y,
				   std::vector<float>& ctrl_pts_x, std::vector<float> ctrl_pts_y, 
				   int canvas_x, int amplitude)
{
	T x;
	for (size_t k{ 1 }; k < ctrl_pts_x.size(); ++k)
	{
		for (size_t i{}; i < std::abs(ctrl_pts_x[k] - ctrl_pts_x[k - 1]); ++i)
		{
			float t = (float)i / (float)std::abs(ctrl_pts_x[k] - ctrl_pts_x[k - 1]);
			float y = std::cos(t*(22/7)) * (amplitude/2) * (k%2==0?-1:1) + (canvas_x/2);
			assert(frames_x.size() > ctrl_pts_x[k - 1] + i);
			frames_y[ctrl_pts_x[k - 1] + i] = y;
			frames_x[ctrl_pts_x[k - 1] + i] = ctrl_pts_x[k-1] + i;
		}
	}
}

template<class T=int>
void get_step_drawn(std::vector<float>& frames_x, std::vector<float>& frames_y, 
				    std::vector<float>& ctrl_pts_x, std::vector<float>& ctrl_pts_y, 
					int canvas_x)
{
	T x;
	for (size_t i{ 1 }; i < ctrl_pts_x.size(); i++)
	{
		for (size_t j{}; j < std::abs(ctrl_pts_x[i] - ctrl_pts_x[i-1]); ++j)
		{
			assert(frames_x.size() > ctrl_pts_x[i - 1] + j);
			frames_y[ctrl_pts_x[i - 1] + j] = ctrl_pts_y[i - 1];
			frames_x[ctrl_pts_x[i - 1] + j] = ctrl_pts_x[i-1] + j;
		}
	}
}

template <typename T = ImVec2>
void get_catmull_rom_drawn(std::vector<float>& frames_x, std::vector<float>& frames_y,
						   std::vector<float> ctrl_pts_x, std::vector<float> ctrl_pts_y,
						   int canvas_x)
{
	if (ctrl_pts_x.size() < 4) {
		return;
	}

	// duplicate the starting point and end points because
	// given 4 points, catmull rom interpolates between the 
	// middle two and the outer two are basically control points
	ctrl_pts_x.insert(ctrl_pts_x.begin(), ctrl_pts_x.front());
	ctrl_pts_x.push_back(ctrl_pts_x.back());

	ctrl_pts_y.insert(ctrl_pts_y.begin(), ctrl_pts_y.front());
	ctrl_pts_y.push_back(ctrl_pts_y.back());

	auto catmull_rom = [](float a, float b, float c, float d, float t)
	{
		return 0.5f * ((-a + 3 * b - 3 * c + d) * t * t * t
					  + (2 * a - 5 * b + 4 * c - d) * t * t
					  + (-a + c) * t
					  + 2 * b);
	};

	auto uniform_catmull_rom = [&](float x0, float x1, float x2, float x3,
								   float y0, float y1, float y2, float y3)
	{
		for (size_t i{}; i < canvas_x; ++i)
		{
			float t = (float)i / (float)canvas_x;

			int x = catmull_rom(x0, x1, x2, x3, t);
			if (x >= frames_x.size() || x < 0) 
				continue;

			float y = catmull_rom(y0, y1, y2, y3, t);

			frames_x[x] = x;
			frames_y[x] = y;
		}
	};

	for (size_t i{ 3 }; i < ctrl_pts_x.size(); ++i)
	{
		uniform_catmull_rom(ctrl_pts_x[i - 3], ctrl_pts_x[i - 2], ctrl_pts_x[i - 1], ctrl_pts_x[i],
							ctrl_pts_y[i - 3], ctrl_pts_y[i - 2], ctrl_pts_y[i - 1], ctrl_pts_y[i]);
	}
}


template <typename T = float>
void get_linear_drawn(std::vector<float>& frames_x, std::vector<float>& frames_y,
								  std::vector<float>& ctrl_pts_x, std::vector<float>& ctrl_pts_y, 
								  int canvas_x)
{
	auto linear_interp= [](float start, float end, float t)
	{
		return ((1.0f - t) * start) + (t * end);
	};

	for (int k{ 1 }; k < ctrl_pts_x.size(); ++k)
	{
		for (int i{}; i < std::abs(ctrl_pts_x[k] - ctrl_pts_x[k - 1]); ++i)
		{
			float t = (float)i / (float)std::abs(ctrl_pts_x[k] - ctrl_pts_x[k - 1]);
			float y = linear_interp(ctrl_pts_y[k - 1], ctrl_pts_y[k], t);
			
			frames_x[ctrl_pts_x[k - 1] + i] = ctrl_pts_x[k - 1] + i;
			frames_y[ctrl_pts_x[k - 1] + i] = y;
		}
	}
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
	
	return points;
}

