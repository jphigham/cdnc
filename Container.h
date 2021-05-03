#pragma once

#include <string>
#include <vector>

#include <rapidjson/document.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Show.h"

class Tile;

class Container
{
public:

	Container();
	virtual ~Container();

	void setName(const char *name) { name_ = name; }
	const std::string &name() { return name_; }

	void get_json_shows(const rapidjson::Value &);

	std::vector<Show> shows_;
	int showIndex_;

private:

	std::string name_;
};
