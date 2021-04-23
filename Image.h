#pragma once

#include <memory>
#include <string>
#include <vector>

class Image
{
public:
	Image();
	virtual ~Image();

	int fetchJpegUrl(const std::string &);

	unsigned int width() { return width_; }
	unsigned int height() { return height_; }
	unsigned char *data() { return reinterpret_cast<unsigned char *>(data_.get()); }

private:
    unsigned int width_, height_;
    std::unique_ptr<unsigned[]> data_;
};
