#include "stdafx.h"
#include "globals.h"
#include "geometry.h"
#include "memorypool.h"
#include "eventhandler.h"

#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Box* character_create(MemoryPool *pool, int x, int y, int w, int h) 
{
	Box* box = PUSH_OBJECT(pool, Box);
	if (!box) { ERROR_REPORT; }

	box->clicked = 0;
	box->x_min = x;
	box->y_min = y;
	box->w = w;
	box->h = h;
	box->x_max = x + w;
	box->y_max = y + h;
	box->target_pos = 0;
	box->r = 1.0;
	box->g = 1.0;
	box->b = 1.0;
	box->a = 1.0;
	box->wait_time = 0.0;
	box->speed = 20.0;

	return box;
}

Box *geometry_box_create(int x, int y, int w, int h)
{
	Box* box = PUSH_OBJECT(&memory_pool, Box);
	if (!box) { ERROR_REPORT; }

	box->clicked = 0;
	box->x_min = x;
	box->y_min = y;
	box->w = w;
	box->h = h;
	box->x_max = x + w;
	box->y_max = y + h;
	box->target_pos = 0;
	box->r = 1.0;
	box->g = 1.0;
	box->b = 1.0;
	box->a = 1.0;
	box->speed = 20.0;

	return box;
}

void geometry_box_ID(Box* box, const char* ID) 
{
	if (!box) { ERROR_REPORT; }
	strcpy(box->name, ID);
}

void geometry_box_position(Box *box, int x, int y)
{
	if (!box) { ERROR_REPORT; }

	box->x_min = x;
	box->y_min = y;

	// move bottom corner by w, h
	box->x_max = box->x_min + box->w;
	box->y_max = box->y_min + box->h;
}

void geometry_box_texture(Box *box, const char *file_name)
{
	if (!box) { ERROR_REPORT; }

	uchar *img = stbi_load(file_name, &box->sprite.w, &box->sprite.h, 0, 4); if (!img) { ERROR_REPORT; }
	glGenTextures(1, &box->sprite.texture);
	glBindTexture(GL_TEXTURE_2D, box->sprite.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA
			   , box->sprite.w, box->sprite.h
			   , 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(img);
	glBindTexture(GL_TEXTURE_2D, 0);

	geometry_box_scale(box);
}

void geometry_box_color(Box *box, float r, float g, float b, float a)
{
	if (!box) { ERROR_REPORT; }
	box->r = r;
	box->g = g;
	box->b = b;
	box->a = a;
}

void geometry_box_size(Box *box, int w, int h)
{
	if (!box) { ERROR_REPORT; }
	box->x_max = box->x_min + w;
	box->y_max = box->y_min + h;
	box->w = w;
	box->h = h;
}

void geometry_box_scale(Box *box)
{
	geometry_box_size(box, box->sprite.w, box->sprite.h);
	if (box->h > game_state->window_height * 0.7) {
		int start = box->h;
		box->h /= (box->h / (game_state->window_height * 0.7));
		box->w = ((box->h * box->w)/start);
	}
}

void geometry_box_draw(Box *box)
{
	if (!box) { ERROR_REPORT; }
	glBindTexture(GL_TEXTURE_2D, box->sprite.texture);
	glBegin(GL_QUADS);
	{
		glColor4f(box->r, box->g, box->b, box->a);

		glTexCoord2d(0.0, 0.0);
		glVertex2f(box->x_min, box->y_min);

		glTexCoord2d(1.0, 0.0);
		glVertex2f(box->x_max, box->y_min);

		glTexCoord2d(1.0, 1.0);
		glVertex2f(box->x_max, box->y_max);

		glTexCoord2d(0.0, 1.0);
		glVertex2f(box->x_min, box->y_max);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
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


// only gets called if an event calls it
char geometry_box_move(Box *box, int x, int y, int speed)
{
	if (!box) { ERROR_REPORT; }
	int s = box->speed;
	double hyp = fabs((double)x - box->x_min);
	if (s > hyp) {
		s = hyp - (hyp / 2);
	}
	if (fabs((double)box->x_min - x) > 5) { // didn't reach the target
		if (box->x_min > x)
			geometry_box_position(box, box->x_min - s, box->y_min);
		else
			geometry_box_position(box, box->x_min + s, box->y_min);
	} else {
		return 0;
	}
	return 1;
}

char geometry_box_wait(double seconds) 
{
	static clock_t start, stop;

	static char init_time = 0;
	if (!init_time) {
		start = clock();
		init_time = 1;
	} else {
		stop = clock() - start;
		double elapsed_time = (double)stop / CLOCKS_PER_SEC;
		
		if (elapsed_time > seconds) {
			init_time = 0;
			stop = start;
			return 0;
		}
	}
	return 1;
}

char geometry_box_fade(Box* box, float speed) {
	if (box->a < 1.0) {
		box->a += 0.05;
	} else {
		return 0;
	}
	return 1;
}

char geometry_box_check_collision(Box *box)
{
	if (!box) { ERROR_REPORT; }
	double cursor_x, cursor_y;
	glfwGetCursorPos(game_state->current_window, &cursor_x, &cursor_y);
	return cursor_x > box->x_min && cursor_x < box->x_max
		&& cursor_y > box->y_min && cursor_y < box->y_max;
}

void geometry_box_on_click(Box *box)
{
	if (!box) { ERROR_REPORT; }
	if (cursor->clicked && geometry_box_check_collision(box)) {
		box->clicked = 1;
		cursor->clicked = 0;
		cursor->holding_left_mouse = 1;
		game_state->last_clicked_box = box;

		// store cursor pos when box is clicked
		glfwGetCursorPos(game_state->current_window, &cursor->click_pos_x, &cursor->click_pos_y);

		// store box position when clicked 
		box->x_pos_when_clicked = box->x_min;
		box->y_pos_when_clicked = box->y_min;
	}

	if (cursor->holding_left_mouse && box->clicked) {
		glfwGetCursorPos(game_state->current_window, &cursor->current_x, &cursor->current_y);
		geometry_line_draw(cursor->click_pos_x, cursor->click_pos_y, cursor->current_x, cursor->current_y);

		// take the cursor's delta_pos from the box's original position 
		float cursor_delta_x = cursor->click_pos_x - cursor->current_x;
		float cursor_delta_y = cursor->click_pos_y - cursor->current_y;
		geometry_box_position(box, box->x_pos_when_clicked - cursor_delta_x
					        , box->y_pos_when_clicked - cursor_delta_y);

	} else {
		box->clicked = 0;
	}
}