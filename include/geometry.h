#pragma once
#include <utility>
#include <array>
#include <vector>
#include <string>
#include <functional>

typedef unsigned char uchar;


struct Sprite
{
	unsigned int	   texture{};
	std::array<int, 2> w_h{};
	std::string		   path;
};

struct Textured 
{
	Sprite						sprite{}; // not needed
	std::vector<Sprite>			sprites{};
	std::vector<std::string>	sprite_paths{ {} };
	unsigned int				sprite_paths_idx{};

	void set_texture(const std::string& fullpath);
};

struct Physical : public Textured
{
	std::array<int, 2>		min_xy{0,0};
	std::array<int, 2>		max_xy{2,2};
	std::array<int, 2>      center{ 1,1 };
	std::array<int, 2>		w_h{2,2};
	std::array<float, 4>	rgba{ 1.0f, 1.0f, 1.0f, 0.0f };

	std::function<void(Physical&,int,int)> setpos = &Physical::set_pos;
	std::function<void(Physical&, std::array<float, 4>)> setcolor = &Physical::set_color;
	std::function<void(Physical&, int)> setx = &Physical::set_x;

	void set_pos(int x, int y)
	{
		center[0] = x;
		center[1] = y;
		min_xy[0] = x - w_h[0]*0.5f;
		min_xy[1] = y - w_h[1]*0.5f;
		max_xy[0] = x + w_h[0]*0.5f;
		max_xy[1] = y + w_h[1]*0.5f;
	}

	inline void set_x(int x)
	{
		set_pos(x, center[1]);
	}

	inline void set_y(int y)
	{
		set_pos(center[0], y);
	}
	
	inline void set_to_sprite_size()
	{
		set_size(sprite.w_h[0], sprite.w_h[1]);
	}

	inline void set_size(int width, int height)
	{
		w_h = std::array{ width, height };
		max_xy[0] = min_xy[0] + width;
		max_xy[1] = min_xy[1] + height;
		center[0] = min_xy[0] + w_h[0] * 0.5f;
		center[1] = min_xy[1] + w_h[1] * 0.5f;
	}

	void scale_to_screen();

	inline void set_color(std::array<float, 4>& color)
	{
		rgba = color;
	}

	inline void set_alpha(float alpha)
	{
		rgba[3] = alpha;
	}

	void draw();
};

struct Animatable : public Physical
{
	float wait_time{ 0.0f };
	float speed{ 5.0f };
	float fade_speed{ 0.05f };
	int	  target_pos{0};

	bool move(const int x, const int y, const int speed);
	bool wait(const double seconds);
	bool fade(const float speed);
};

struct Character : public Animatable
{
	std::string name{};

	Character() = default;
	Character(const std::string& name) {
		this->name = name;
	}
	Character(const std::string& name, std::string& spritepath) {
		this->name = name;
		set_texture(spritepath);
		set_to_sprite_size();
	}
	Character(const int x_min, const int y_min, const int w, const int h) 
	{  
		min_xy  = std::array{ x_min, y_min };
		w_h		= std::array{ w, h };
		max_xy  = std::array{ x_min + w, y_min + h };
		center[0] = min_xy[0] + w_h[0] * 0.5f;
		center[1] = min_xy[1] + w_h[1] * 0.5f;
	}

	std::string to_string()
	{
		return name + " xpos " + std::to_string(min_xy[0]) 
				    + " alpha " + std::to_string(rgba[3]);
	}
};



