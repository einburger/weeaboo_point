#include <string>

#include "globals.h"
#include "geometry.h"
#include "fileloader.h"
#include "gamestate.h"

void Textured::set_texture(const std::string &fullpath) {
    sprite = fileloader::load_sprite(fullpath);
    for (const auto &tex : sprites) {
        if (tex.texture == sprite.texture) 
            return;
    }
    sprites.push_back(sprite);
}

void Physical::scale_to_screen() {
	set_size(sprite.w_h[0], sprite.w_h[1]);
	const int start = w_h[1];
	w_h[1] = GameState::w_h[1] * 0.9;
	w_h[0] = ((w_h[0] * w_h[1]) / start);
}

void Physical::draw()
{
	glBindTexture(GL_TEXTURE_2D, sprite.texture);
	glBegin(GL_QUADS);
	{
		glColor4f(rgba[0], rgba[1], rgba[2], rgba[3]);

		glTexCoord2d(0.0, 0.0); // top left
		glVertex2f(min_xy[0], min_xy[1]);

		glTexCoord2d(1.0, 0.0); // top right
		glVertex2f(max_xy[0], min_xy[1]);

		glTexCoord2d(1.0, 1.0); // bottom right
		glVertex2f(max_xy[0], max_xy[1]);

		glTexCoord2d(0.0, 1.0); // bottom left
		glVertex2f(min_xy[0], max_xy[1]);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}



// only gets called if an event calls it
bool Animatable::move(int x, int y, int speed)
{
	int s = speed;
	double hyp = std::abs((double)x - center[0]);

	if (s > hyp)
	{
		s = hyp - (hyp / 2);
	}

	if (std::abs((double)center[0] - x) > 5)
	{ // didn't reach the target
		center[0] > x ? set_pos(center[0] - s, center[1]) 
				  	  : set_pos(center[0] + s, center[1]);
	}
	else
	{
		return false;
	}

	return true;
}

bool Animatable::wait(double seconds)
{
	static clock_t start, stop;

	static bool init_time = false;
	if (!init_time)
	{
		start = clock();
		init_time = true;
	}
	else
	{
		stop = clock() - start;
		double elapsed_time = (double)stop / CLOCKS_PER_SEC;

		if (elapsed_time > seconds)
		{
			init_time = false;
			stop = start;
			return false;
		}
	}
	return true;
}

bool Animatable::fade(float speed)
{ // negative speed means fadout
	auto fade_in = [&](){
		if (rgba[3] < 1.0) {
			rgba[3] += speed;
			return true;
		}
		return false;
	};

	auto fade_out = [&](){
		if (rgba[3] > 0.0) {
			rgba[3] += speed;
			return true;
		}
		return false;
	};

	return speed < 0 ? fade_out() : fade_in();
}

