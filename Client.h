#pragma once

#include <memory>
#include <string>
#include <vector>


#include "Container.h"

class Text;
class Tile;

// Client manages fetching the home page from the data API
// and populating the display

class Client
{
public:
	Client(unsigned, unsigned);
	virtual ~Client();
	void init();
	void load();
	void draw();
	void moveCursor(int);
private:
	int read_json(const char *, std::unique_ptr<std::string> &);
	void parse_json(const std::unique_ptr<std::string> &);

	bool verbose_;

	unsigned width_, height_;

	std::vector<Container> containers_;
	std::vector<Container>::iterator currentContainer_;
	unsigned startContainer_;
	int cursorX_, cursorY_;

	Tile *tile_;
	Text *text_;

	const int gridWidth_ = 5;
	const int gridHeight_ = 4;
	const unsigned int textHeight_ = 24;
};
