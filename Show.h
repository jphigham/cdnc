#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Tile;

class Show {
public:
	Show();
	virtual ~Show();

	void setName(const char *name) { name_ = name; }
	const std::string &name() { return name_; }
	void setMasterId(const char *masterId) { masterId_ = masterId; }
	const std::string &masterId() { return masterId_; }
	void setUrl(const char *url) { url_ = url; }
	const std::string &url() { return url_; }

	void draw(Tile *, glm::vec2 position, glm::vec2 size);

private:
	std::string name_;
	std::string masterId_;
	std::string url_;

	bool failedUrl_;
};
