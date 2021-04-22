#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class Curl
{
public:
	static int fetch(const char *, const std::filesystem::path &);
	static int fetch(const char *, std::unique_ptr<std::string> &);
	static int fetch(const char *, std::unique_ptr<std::vector<char>> &);
private:
	Curl() {}
};

