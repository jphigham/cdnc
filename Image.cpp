#include "Image.h"

#include <iostream>

#include "Curl.h"
#include "jpeg.h"

Image::Image()
	// NOTE: size can't be hardcoded
	: width_(500), height_(281)
{
	data_ = std::make_unique<unsigned[]>(width_ * height_);
}

Image::~Image()
{
	// std::cout << "Image::~Image(" << std::hex << this << ")" << std::endl;
}

int Image::fetchJpegFile(const std::string &path)
{
	return 0;
}

int Image::fetchJpegUrl(const std::string &url)
{
	// NOTE: This should be reused and not reallocated
	auto jpeg_buffer = std::make_unique<std::vector<char>>();
	if (Curl::fetch(url.c_str(), jpeg_buffer))
		return -1;
	if (jpeg_read_mem(reinterpret_cast<const unsigned char*>(jpeg_buffer.get()->data()), jpeg_buffer->size(),
			&width_, &height_,
			reinterpret_cast<unsigned char*>(data_.get())
			// data()
			))
		return -1;
	return 0;
}
