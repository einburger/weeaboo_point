#include "stdafx.h"
#include "text.h"
#include "scene.h"
#include "globals.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static unsigned char ttf_buffer[1 << 20];
static unsigned char temp_bitmap[512 * 512];
static stbtt_bakedchar cdata[96];

static unsigned char ttf_buffer_2[1 << 20];
static unsigned char temp_bitmap_2[512 * 512];
static stbtt_bakedchar cdata_2[96];

static GLuint ftex, btex;

void text_init(const char* path, float sz)
{
	char text_path[256];
	text_append(text_path, FONT_PATH, (char*)path);

	fread(ttf_buffer, 1, 1 << 20, fopen(text_path, "rb"));
	stbtt_BakeFontBitmap(ttf_buffer, 0, sz, temp_bitmap, 512, 512, 32, 96, cdata);
	glGenTextures(1, &ftex);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// black text background until i create shaders
	fread(ttf_buffer_2, 1, 1 << 20, fopen(text_path, "rb"));
	stbtt_BakeFontBitmap(ttf_buffer_2, 0, sz, temp_bitmap_2, 512, 512, 32, 96, cdata_2);
	glGenTextures(1, &btex);
	glBindTexture(GL_TEXTURE_2D, btex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap_2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*
std::string text_justify(std::string& text, int max_count)
{
	std::string final_text;
	int chrcnt = 0;
	for (size_t i{}; i < text.size(); i++) {
		chrcnt++;
		if (chrcnt > max_count) {
			final_text += "\n";
			chrcnt = 0;
		}
		final_text += text[i];
	}
	return final_text;
}
*/

void text_render(const char* text, float x, float y, int COLOR)
{
	glBindTexture(GL_TEXTURE_2D, ftex);

	switch (COLOR)
	{
	case BLACK:
		glColor4f(0.0, 0.0, 0.0, 1.0);
		break;
	case WHITE:
		glColor4f(1.0, 1.0, 1.0, 1.0);
		break;
	}

	glBegin(GL_QUADS);
	{
		for (size_t i = 0; i < game_state->text_cursor_pos; ++i)
		{
			if (*text)
			{
				if (*text >= 32 && *text < 128)
				{
					static stbtt_aligned_quad q;
					stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);
					glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
					glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
					glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
					glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
				}
				++text;
			}
		}
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void text_draw(const char* line)
{
	float xpos = game_state->scene->textbox->x_min + 180;
	float ypos = game_state->scene->textbox->y_max - 68;

	/* black text moved by 2 in each cardinal direction to create
	 * to create a black text border */
	text_render(line, xpos + 2, ypos + 2, BLACK);
	text_render(line, xpos - 2, ypos - 2, BLACK);
	text_render(line, xpos + 2, ypos - 2, BLACK);
	text_render(line, xpos - 2, ypos + 2, BLACK);
	text_render(line, xpos, ypos - 2, BLACK);
	text_render(line, xpos, ypos + 2, BLACK);
	text_render(line, xpos + 2, ypos, BLACK);
	text_render(line, xpos - 2, ypos, BLACK);

	text_render(line, xpos, ypos, WHITE);
}

void text_append(char* destination, char* string_a, char* string_b)
{
	strcpy(destination, string_a);
	strcat(destination, string_b);
}
