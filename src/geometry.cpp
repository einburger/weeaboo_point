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
	set_size(sprite.wdth_hght);
	float start = w_h.y;
	w_h.y = GameState::w_h.x * 0.5f;
	w_h.x = ((w_h.x * w_h.y) / start);
}

void Physical::draw() {
	glBindTexture(GL_TEXTURE_2D, sprite.texture);
	glBegin(GL_QUADS);
	{
		glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);

		glTexCoord2d(0.0, 0.0); // top left
		glVertex2f(min_xy.x, min_xy.y);

		glTexCoord2d(1.0, 0.0); // top right
		glVertex2f(max_xy.x, min_xy.y);

		glTexCoord2d(1.0, 1.0); // bottom right
		glVertex2f(max_xy.x, max_xy.y);

		glTexCoord2d(0.0, 1.0); // bottom left
		glVertex2f(min_xy.x, max_xy.y);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}



// only gets called if an event calls it
bool Animatable::move(int x, int y, int speed) {
	int s = speed;
	double hyp = std::abs((double)x - center.x);

	if (s > hyp)
	{
		s = hyp - (hyp / 2);
	}

	if (std::abs((double)center.x - x) > 5)
	{ // didn't reach the target
		center.x > x ? set_pos(center.x - s, center.y) 
				  	 : set_pos(center.x + s, center.y);
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
	std::cout << "entered fade func\n";
	auto fade_in = [&](){
		if (rgba.a < 1.0f) {
			rgba.a += speed;
			return true;
		}
		return false;
	};

	auto fade_out = [&](){
		if (rgba.a > 0.0f) {
			rgba.a += speed;
			return true;
		}
		return false;
	};

	return speed < 0.0f ? fade_out() : fade_in();
}

