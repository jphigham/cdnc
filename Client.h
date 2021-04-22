#pragma once

#include <map>
#include <memory>
#include <string>

#include <rapidjson/document.h>

// Client manages fetching the home page from the data API
// and populating the display

class Client
{
public:
	Client();
	~Client();
	void init();
	void draw();
private:
	int read_json(const char *, std::unique_ptr<std::string> &);
	void parse_json(const std::unique_ptr<std::string> &);
	void get_json_items(const rapidjson::Value &);

};
