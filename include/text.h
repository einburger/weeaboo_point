#pragma once

void		text_init(const char* text_path, float font_size);
void            text_render(const char* sentence, float x, float y, int COLOR);
void		text_draw(const char* sentence);
char		text_write_animation(const char* text);
