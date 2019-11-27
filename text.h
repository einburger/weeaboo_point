#pragma once

void		text_init(const char *text_path, float font_size);
//std::string text_justify(std::string& text, int max_count);
inline void text_append(char* destination, char *string_a, char *string_b);
static void text_render(const char *sentence, float x, float y, int COLOR);
void		text_draw(const char *sentence);
