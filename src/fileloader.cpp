#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>

#include "fileloader.h"
#include "globals.h"
#include "gamestate.h"
#include "geometry.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fileloader {

	static std::unordered_map<std::string, std::vector<std::string> > sprite_paths;

	const std::vector<std::string>& get_paths(const std::string& name) { 
		return sprite_paths[name]; 
	}

	// build a vector of paths to sprites of a specific folder / asset
	void sniff_files()
	{
		auto& scene = GameState::scene;
		for (const auto& file : fs::directory_iterator(CHARACTER_PATH))
		{
			if (fs::is_directory(file.status()))
			{
				const auto character_name = file.path().filename().string();

				std::vector<std::string> paths;
				for (const auto& sub_file : fs::directory_iterator(file.path()))
				{
					paths.push_back(sub_file.path().string());
				}
				sprite_paths[character_name] = paths;
			}
		}

		std::vector<std::string> paths;
		for (const auto& file : fs::directory_iterator(BG_PATH))
		{
			paths.push_back(file.path().string());
		}
		sprite_paths["bg"] = paths;
	}

	Sprite load_sprite(const std::string& fullpath)
	{
		static std::unordered_map<std::string, Sprite> sprite_cache;

		if (auto it = sprite_cache.find(fullpath); it != sprite_cache.end())
		{
			return it->second;
		}

		// have to read it in from disk 
		return [&]()
		{
			Sprite spr{};

			unsigned char* img = stbi_load(fullpath.c_str(), &spr.wdth_hght.x, &spr.wdth_hght.y, 0, 4);
			glGenTextures(1, &spr.texture);
			glBindTexture(GL_TEXTURE_2D, spr.texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr.wdth_hght.x, spr.wdth_hght.y,
						 0, GL_RGBA, GL_UNSIGNED_BYTE,
						 img);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(img);

			sprite_cache[fullpath] = spr;

			return spr;
		}();
	}

} // end namespace

