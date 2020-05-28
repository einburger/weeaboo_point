#pragma once
#include <utility>
#include <array>
#include <vector>
#include <string>
#include <functional>

#include "utils.h"

struct Sprite
{
	unsigned int	texture{};
	v2<int>			wdth_hght{};
	std::string		path;
};

struct Textured
{
	Sprite						sprite{};
	std::vector<Sprite>			sprites{};
	std::vector<std::string>	sprite_paths{ {} };
	unsigned int				sprite_paths_idx{};

	void set_texture(const std::string& fullpath);
};

struct Physical : public Textured
{
	v2<int>		min_xy{ 0,0 };
	v2<int>		max_xy{ 2,2 };
	v2<int>     center{ 1,1 };
	v2<int>		w_h{ 2,2 };
	v4<float>	rgba{ 1.0f, 1.0f, 1.0f, 0.0f };

	inline void set_pos(const v2<int>& other)
	{
		set_pos(other.x, other.y);
	}

	inline void set_x(int x)
	{
		set_pos(x, center.y);
	}

	inline void set_y(int y)
	{
		set_pos(center.x, y);
	}

	inline void set_pos(int x, int y)
	{
		center = { x, y };
		min_xy = center - (w_h * 0.5f);
		max_xy = center + (w_h * 0.5f);
	}

	inline void set_to_sprite_size()
	{
		set_size(sprite.wdth_hght);
	}

	inline void set_size(const v2<int>& other_w_h)
	{
		set_size(other_w_h.x, other_w_h.y);
	}

	inline void set_size(int width, int height)
	{
		w_h = { width, height };
		max_xy = min_xy + w_h;
		center = min_xy + (w_h * 0.5f);
	}

	void scale_to_screen();

	inline void set_color(const v4<float>& color)
	{
		rgba = color;
	}

	inline void set_color(float a, float b, float c, float d)
	{
		rgba.r = a, rgba.g = b, rgba.b = c, rgba.a = d;
	}

	inline void set_alpha(float alpha)
	{
		rgba.a = alpha;
	}

	void draw();
};

struct Character : Physical
{
	std::string name{};
	Character() = default;
	Character(const std::string& name)
	{
		this->name = name;
	}
	Character(const std::string& name, std::string& spritepath)
	{
		this->name = name;
		set_texture(spritepath);
		set_to_sprite_size();
	}
	Character(const int x_min, const int y_min, const int w, const int h)
	{
		min_xy = { x_min, y_min };
		w_h = { w, h };
		max_xy = min_xy + w_h;
		center = min_xy + (w_h * 0.5f);
	}

	std::string to_string()
	{
		return name + " xpos " + std::to_string(center.x)
			+ " alpha " + std::to_string(rgba.a);
	}
};

