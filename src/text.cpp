#include "globals.h" // path

#include "text.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static unsigned char ttf_buffer[1 << 20];
static unsigned char temp_bitmap[512 * 512];
static stbtt_bakedchar cdata[96];

static unsigned char ttf_buffer_2[1 << 20];
static unsigned char temp_bitmap_2[512 * 512];
static stbtt_bakedchar cdata_2[96];

static GLuint ftex, btex;

void Field::init(const char* path, float sz)
{
	std::string text_path = FONT_PATH + path;

	fread(ttf_buffer, 1, 1 << 20, fopen(text_path.c_str(), "rb"));
	stbtt_BakeFontBitmap(ttf_buffer, 0, sz, temp_bitmap, 512, 512, 32, 96, cdata);
	glGenTextures(1, &ftex);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// black text background until i create shaders
	fread(ttf_buffer_2, 1, 1 << 20, fopen(text_path.c_str(), "rb"));
	stbtt_BakeFontBitmap(ttf_buffer_2, 0, sz, temp_bitmap_2, 512, 512, 32, 96, cdata_2);
	glGenTextures(1, &btex);
	glBindTexture(GL_TEXTURE_2D, btex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap_2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

}

bool Field::write_animation()
{
	static clock_t start, stop;
	static char init_time = 0;
	if (GameState::text_cursor_pos > GameState::scene.dialog.size())
	{
		GameState::waiting_for_input = true;
		return false;
	}

	if (!init_time)
	{
		start = clock();
		init_time = 1;
	}
	else
	{
		stop = clock() - start;
		double elapsed_time = (double)stop / CLOCKS_PER_SEC;

		if (elapsed_time > 0.5f)
		{
			dl.draw();
			init_time = 0;
			stop = start;
		}
	}
	return true;
}

void Line::draw()
{
	char* text = (char*)line.c_str();

	glBindTexture(GL_TEXTURE_2D, ftex);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	int start_x = x_pos;
	glBegin(GL_QUADS);
	{
		for (size_t i = 0; i < local_cursor_pos; ++i)
		{
			if (*text)
			{
				if (*text >= 32 && *text < 128)
				{
					static stbtt_aligned_quad q;
					stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x_pos, &y_pos, &q, 1);
					glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
					glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
					glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
					glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
				}
				++text;
			}
		}
	}
	x_pos = start_x;
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DialogLines::draw()
{
	for (size_t i{}; i < lines.size(); ++i)
	{
		lines[i].draw();
		if (lines[i].local_cursor_pos <= lines[i].line.size())
		{
			lines[i].local_cursor_pos++;
			GameState::text_cursor_pos++;
			return;
		}
	}
}

