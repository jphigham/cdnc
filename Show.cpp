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

void Show::validate()
{
	if (masterId_.length() > 64) {
		auto tmp = masterId_.substr(0, 64);
		auto pos = url_.find(masterId_);
		url_.replace(pos, masterId_.length(), tmp);
		masterId_ = tmp;
	}
}

void Show::draw(Tile *tile, glm::vec2 position, glm::vec2 size, bool drawNormal)
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
	const glm::vec3 dim(0.3f);
	const glm::vec3 normal(1.0f);
	tile->Draw(ResourceManager::GetTexture(masterId_), position, size, 0.f, drawNormal ? normal : dim);
}
