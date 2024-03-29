#include "Image.h"

#include <iostream>

#include "Curl.h"
#include "jpeg.h"

Image::Image()
	// NOTE: size shouldn't be hardcoded
	: width_(500), height_(400)
{
	data_ = std::make_unique<unsigned[]>(width_ * height_);
}

Image::~Image()
{
	// std::cout << "Image::~Image(" << std::hex << this << ")" << std::endl;
}

int Image::fetchJpegUrl(const std::string &url)
{
	// NOTE: This should be reused and not reallocated
	auto jpeg_buffer = std::make_unique<std::vector<char>>();
	Curl curl;
	if (curl.fetch(url.c_str(), jpeg_buffer))
		return -1;
	if (jpeg_read_mem(reinterpret_cast<const unsigned char*>(jpeg_buffer.get()->data()), jpeg_buffer->size(),
			&width_, &height_,
			reinterpret_cast<unsigned char*>(data_.get())
			// data()
			))
		return -1;
	return 0;
}
