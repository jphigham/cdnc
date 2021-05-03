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

	void setShowModal(bool showModal );
	bool showModal() const { return showModal_; }
	void moveCursor(int);

	int gridWidth() const { return gridWidth_; }
	void setGridWidth(int newGridWidth) { gridWidth_ = newGridWidth; }
	int gridHeight() const { return gridHeight_; }
	void setGridHeight(int newGridHeight) { gridHeight_ = newGridHeight; }

private:
	int read_json(const char *, std::unique_ptr<std::string> &);
	void parse_json(const std::unique_ptr<std::string> &);

	bool verbose_;
	bool showModal_;

	unsigned int width_, height_;

	std::vector<Container> containers_;
	std::vector<Container>::iterator currentContainer_;
	int containerIndex_;
	int cursorX_, cursorY_;

	Tile *tile_;
	Text *text_;

	int gridWidth_;
	int gridHeight_;

	const unsigned int textHeight_ = 24;
};
