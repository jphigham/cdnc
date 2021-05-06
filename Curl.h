#pragma once

#include <curl/curl.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class Curl
{
public:
	Curl();
	~Curl();
	int fetch(const char *, const std::filesystem::path &);
	int fetch(const char *, std::unique_ptr<std::string> &);
	int fetch(const char *, std::unique_ptr<std::vector<char>> &);
private:
	void fetch_init();
	void fetch_set(const char *url);
	int fetch_execute();
	CURL *curl_;
};

