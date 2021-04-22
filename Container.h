#pragma once

#include <string>

class Container
{
public:
	Container();
	~Container();

	void setName(const char *name) { name_ = name; }
	const std::string &name() { return name_; }
private:
	std::string name_;
};
