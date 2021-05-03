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


	void setRelease(const char *release) { release_ = release; }
	const std::string &release() { return release_; }
	void setRating(const char *rating) { rating_ = rating; }
	const std::string &rating() { return rating_; }

	void validate();

	void draw(Tile *, glm::vec2 position, glm::vec2 size, bool drawNormal = true);

private:
	std::string name_;
	std::string masterId_;
	std::string url_;

	std::string release_;
	std::string rating_;

	bool failedUrl_;
};
