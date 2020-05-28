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

