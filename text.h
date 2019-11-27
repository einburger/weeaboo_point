#pragma once

void		text_init(const char* text_path, float font_size);
inline void text_append(char* destination, char* string_a, char* string_b);
static void text_render(const char* sentence, float x, float y, int COLOR);
void		text_draw(const char* sentence);
