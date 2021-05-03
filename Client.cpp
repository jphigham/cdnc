#include "Client.h"

#include "data_api.h"
#include "resource_path.h"

#include <fstream>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Curl.h"
#include "parse_json_utils.h"

#include "ResourceManager.h"
#include "Show.h"
#include "Text.h"
#include "Tile.h"

Client::Client(unsigned int width, unsigned int height)
	: verbose_(false), showModal_(false),
	  width_(width), height_(height),
	  gridWidth_(5), gridHeight_(4),
	  cursorX_(-1), cursorY_(-1),
	  containerIndex_(0),
	  tile_(nullptr),
	  text_(nullptr)
{

}

Client::~Client()
{
	delete tile_;
	delete text_;
	containers_.clear();
}

void Client::init()
{
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	ResourceManager::LoadShader(
			shader_path("tile.vert").string().c_str(),
			shader_path("tile.frag").string().c_str(),
			nullptr, "tile");

	glm::mat4 projection = glm::ortho(0.0f,
    	static_cast<float>(width_), static_cast<float>(height_),
		0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("tile").Use().SetInteger("tile", 0);
    ResourceManager::GetShader("tile").SetMatrix4("projection", projection);

    tile_ = new Tile(ResourceManager::GetShader("tile"));

    text_ = new Text(width_, height_);
    text_->Load(font_path("DejaVuSans.ttf").string(), textHeight_);
	text_->RenderText("Loading...", width_ / 2.f, height_ / 2.f, 1.0f);
}

void Client::load()
{
	auto home_json_data = std::make_unique<std::string>();
	Curl::fetch(home_json_url.c_str(), home_json_data);
	parse_json(home_json_data);
}

void Client::moveCursor(int key)
{
	if (cursorX_ == -1 && cursorY_ == -1) {
		// first movement, set to (0,0)
		cursorX_ = cursorY_ = 0;
	} else
		switch (key) {
		case GLFW_KEY_UP:
			// Disallow negative
			if (cursorY_ > 0)
				cursorY_--;
			// scroll up
			else if (containerIndex_ > 0)
				containerIndex_--;
			break;
		case GLFW_KEY_DOWN:
			if (cursorY_ < gridHeight_ - 1)
				cursorY_++;
			// scroll down
			else if (containerIndex_ < int(containers_.size()) - gridHeight_)
				containerIndex_++;
			break;
		case GLFW_KEY_LEFT:
			// Disallow negative
			if (cursorX_ > 0)
				cursorX_--;
			// scroll current container left
			else if (containers_[cursorY_ + containerIndex_].showIndex_ > 0)
				containers_[cursorY_ + containerIndex_].showIndex_--;
			break;
		case GLFW_KEY_RIGHT:
			if (cursorX_ < gridWidth_ - 1)
				cursorX_++;
			// scroll current container right
			else if (containers_[cursorY_+ containerIndex_].showIndex_ < int(containers_[cursorY_].shows_.size()) - gridWidth_)
				containers_[cursorY_+ containerIndex_].showIndex_++;
			break;
			break;
		default:
			break;
		}
}

void Client::setShowModal(bool showModal )
{
	if (cursorX_ != -1 && cursorY_ != -1)
		showModal_ = showModal;
}

void Client::draw()
{
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // grid rectagles for shows
    glm::vec2 showGridSize = glm::vec2(float(width_) / float(gridWidth_ + 1),
    		float(height_) / float(gridHeight_ + 1));
    // show tile size matching window aspect ratio
    glm::vec2 showSize(showGridSize.x, showGridSize.x * float(height_) / float (width_));
    // top left show position
    glm::vec2 cornerPos(showGridSize / 2.f);
    // loop-controlled drawing position
    glm::vec2 drawPos = cornerPos;

    // loop over grid y range, including off screen
    for (int gy = -1, c = containerIndex_ - 1; gy <= gridHeight_; gy++, c++) {

    	drawPos.y = cornerPos.y + (gy * showGridSize.y);

    	// draw only when when containerIndex_ is within range
    	if (0 <= c && c < int(containers_.size())) {

    		// loop over x range, including off screen
    		for (int gx = -1, s = containers_[c].showIndex_ - 1; gx <= gridWidth_; gx++, s++) {

				drawPos.x = cornerPos.x + (gx * showGridSize.x);

				// draw only when showIndex_ is within range
    			if (0  <= 0 && s < int(containers_[c].shows_.size())) {
    				// scale up focused show
    				float showScale = (gy == cursorY_ && gx == cursorX_) ? 1.05f : 1.0f;
    				glm::vec2 focusOffset = showGridSize * (showScale - 1.0f) / 2.0f;

    				containers_[c].shows_[s].draw(tile_,
    						drawPos - focusOffset, showSize * showScale * .9f,
							// normal display for grid area, dim for off screen indicators
							0 <= gy && gy < gridHeight_ &&
							0 <= gx && gx < gridWidth_);
    			}
    		}

    		if (0 <= gy && gy < gridHeight_)
    			text_->RenderText(containers_[c].name(), cornerPos.x, drawPos.y - textHeight_, .9f);
    	}
    }

    if (showModal_) {
    	float modalScale = 0.8f;
    	glm::vec2 modalSize(width_, height_);
    	glm::vec2 modalPos = modalSize * (1.0f - modalScale) / 2.0f;
    	auto container = containers_[cursorY_ + containerIndex_];
    	auto show = container.shows_[cursorX_ + container.showIndex_];
    	show.draw(tile_, modalPos, modalSize * modalScale);
    }
}

int Client::read_json(const char *filename, std::unique_ptr<std::string> &json_data)
{
	std::ifstream in;
	in.open(filename);
	try {
		in.exceptions(std::ios_base::failbit);
	} catch (const std::ios_base::failure &exception) {
		std::cerr << exception.what() << std::endl;
		return 1;
	}
	std::stringstream stream;
	stream << in.rdbuf();
	json_data.get()->operator=(stream.str());
	return 0;
}


void Client::parse_json(const std::unique_ptr<std::string> &json_data)
{
	rapidjson::Document doc;
	rapidjson::ParseResult ok = doc.Parse(json_data.get()->c_str());
	if (!ok) {
		std::cerr << "JSON parse error (home): "
				<< rapidjson::GetParseError_En(ok.Code())
				<< " ("
				<< ok.Offset()
				<< ")" << std::endl;
		return;
	}

	const rapidjson::Value &containers = doc["data"]["StandardCollection"]["containers"];
	if (containers.IsArray()) {
		if (verbose_)
			std::cout << "Found " << containers.Size() << " containers" << std::endl;

		for (rapidjson::SizeType i = 0; i < containers.Size(); ++i) {

			const rapidjson::Value &container = containers[i];

			Container c;
			if (container["set"].HasMember("collectionId")) {
				const rapidjson::Value &collectionId = container["set"]["collectionId"];
			}
			c.setName(container["set"]["text"]["title"]["full"]["set"]["default"]["content"].GetString());

			if (verbose_)
				std::cout << container["set"]["text"]["title"]["full"]["set"]["default"]["content"].GetString() << std::endl;

			if (container["set"].HasMember("refId")) {
				const rapidjson::Value &refId = containers[i]["set"]["refId"];
				if (verbose_)
					std::cout << " : " << refId.GetString() << std::endl;

				// Read or fetch ref json
				auto ref_json_data = std::make_unique<std::string>();

				std::filesystem::path ref_json_path = json_path("set-refId-");
				ref_json_path += refId.GetString();
				ref_json_path += ".json";

				if (std::filesystem::exists(ref_json_path)) {
					if (verbose_)
						std::cout << "Reading " << ref_json_path << std::endl;
					if (read_json(ref_json_path.string().c_str(), ref_json_data))
						std::cerr << "problem reading" << std::endl;
				} else {
					std::string ref_json_url;
					ref_json_url = ref_json_base_url;
					ref_json_url += refId.GetString();
					ref_json_url += ".json";
					if (verbose_)
						std::cout << "Fetching " << ref_json_url << std::endl;
					if (Curl::fetch(ref_json_url.c_str(), ref_json_data))
						std::cerr << "problem fetching" << std::endl;
				}

				rapidjson::Document ref_doc;
				if (verbose_)
					std::cout << "ref json data size = " << ref_json_data->size() << std::endl;
				rapidjson::ParseResult ok = ref_doc.Parse(ref_json_data->c_str());
				if (ok) {
					rapidjson::Value ref_items;
					if (ref_doc["data"].HasMember("CuratedSet"))
						ref_items = ref_doc["data"]["CuratedSet"]["items"];
					else if (ref_doc["data"].HasMember("PersonalizedCuratedSet"))
						ref_items = ref_doc["data"]["PersonalizedCuratedSet"]["items"];
					else if (ref_doc["data"].HasMember("TrendingSet"))
						ref_items = ref_doc["data"]["TrendingSet"]["items"];
					if (verbose_) {
						std::cout << " - Found " << ref_items.Size() << " items" << std::endl;
						NodePrinter::printItems(ref_items);
					}
					c.get_json_shows(ref_items);
				} else {
					std::cerr << "JSON parse error (ref): "
							<< rapidjson::GetParseError_En(ok.Code())
							<< " ("
							<< ok.Offset()
							<< ")" << std::endl;
				}
			}

			if (container["set"].HasMember("items")) {
				const rapidjson::Value &items = container["set"]["items"];
				if (verbose_) {
					std::cout << " - Found " << items.Size() << " items" << std::endl;
					NodePrinter::printItems(items);
				}
				c.get_json_shows(items);
			}

			containers_.push_back(c);
			if (cursorY_ == -1)
				currentContainer_ = containers_.begin();
		}
	}
}
