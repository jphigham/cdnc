#include "Show.h"

#include <iostream>
#include <memory>

#include "Image.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Tile.h"

Show::Show()
	: failedUrl_(false)
{

}

Show::~Show()
{

}

void Show::draw(Tile *tile, glm::vec2 position, glm::vec2 size)
{
	if (failedUrl_)
		return;

	if (ResourceManager::Textures.find(masterId_) == ResourceManager::Textures.end()) {
		// std::cout << "Fetching image for " << name_ << "... ";
		auto image = std::make_unique<Image>();
		if (image->fetchJpegUrl(url_)) {
			// std::cout << "failed" << std::endl;
			failedUrl_ = true;
			return;
		} else {
			Texture texture;
	        texture.Internal_Format = GL_RGBA;
	        texture.Image_Format = GL_RGBA;
			texture.Generate(image->width(), image->height(), image->data());
			ResourceManager::Textures[masterId_] = texture;
			// std::cout << "good" << std::endl;
		}
	} else {
		// std::cout << "Reusing image for " << name_ << std::endl;
	}

	tile->Draw(ResourceManager::GetTexture(masterId_), position, size);
}
