#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "globals.h"
#include "geometry.h"

void Box::set_texture(const std::string &file_name)
{
	uchar* img = stbi_load(file_name.c_str(), &sprite.w_h[0], &sprite.w_h[1], 0, 4); 

	glGenTextures(1, &sprite.texture);
	glBindTexture(GL_TEXTURE_2D, sprite.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
				 sprite.w_h[0], sprite.w_h[1], 
				 0, GL_RGBA, GL_UNSIGNED_BYTE, 
				 img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glDisable(GL_BLEND);



	stbi_image_free(img);
	glBindTexture(GL_TEXTURE_2D, 0);

	// scale();
}

void Box::set_emotion(const std::string &emotion) {
	if (auto it = emotion_map.find(emotion); it != emotion_map.end()) {
            set_texture(it->second);
	}
}

void Box::scale() { // this downsizes to fit in 70% of screen height  
	set_size(sprite.w_h);
	const int start = w_h[1];
	w_h[1] = game_state->window_height * 0.9;
	w_h[0] = ((w_h[0] * w_h[1]) / start);
}


void Box::draw()
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
bool Box::move(int x, int y, int speed)
{
	int s = speed;
	double hyp = std::abs((double)x - min_xy[0]);

	if (s > hyp)
	{
		s = hyp - (hyp / 2);
	}

	if (std::abs((double)min_xy[0] - x) > 5)
	{ // didn't reach the target
		if (min_xy[0] > x)
                        set_position(min_xy - std::array{s, 0});
		else
			set_position(min_xy + std::array{s, 0});
	}
	else
	{
		return false;
	}

	return true;
}

bool Box::wait(double seconds)
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

bool Box::fade(float speed)
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

void geometry_line_draw(int x0, int y0, int x1, int y1)
{
	glBegin(GL_LINES);
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glVertex2f(x0, y0);
		glVertex2f(x1, y1);
	}
	glEnd();
}

