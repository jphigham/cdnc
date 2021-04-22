#pragma once

#include <memory>
#include <string>
#include <vector>

#include <rapidjson/document.h>

#include "Container.h"

class Text;

// Client manages fetching the home page from the data API
// and populating the display

class Client
{
public:
	Client(unsigned, unsigned);
	~Client();
	void init();
	void draw();
	void moveCursor(int);
private:
	int read_json(const char *, std::unique_ptr<std::string> &);
	void parse_json(const std::unique_ptr<std::string> &);
	void get_json_items(const rapidjson::Value &);

	bool verbose_;

	unsigned width_, height_;
	std::vector<Container> containers_;
	std::vector<Container>::iterator currentContainer_;
	int cursorX_, cursorY_;

	Text *text_;

	const int maxCursorX_ = 5;
	const int maxCursorY_ = 4;
};
