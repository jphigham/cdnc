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
	: verbose_(false),
	  width_(width), height_(height),
	  cursorX_(-1), cursorY_(-1),
	  startContainer_(0),
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
	auto home_json_data = std::make_unique<std::string>();
	Curl::fetch(home_json_url.c_str(), home_json_data);
	parse_json(home_json_data);

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
    text_->Load(font_path("Antonio-Regular.ttf").string(), 24);
}

void Client::moveCursor(int key)
{
	if (cursorX_ == -1 && cursorY_ == -1) {
		// first movement, set to (0,0)
		cursorX_ = cursorY_ = 0;
	} else
		switch (key) {
		case GLFW_KEY_LEFT:
			// Disallow negative
			if (cursorX_ > 0)
				cursorX_--;
			break;
		case GLFW_KEY_RIGHT:
			if (cursorX_ < gridWidth_ - 1)
				cursorX_++;
			break;
		case GLFW_KEY_UP:
			// Disallow negative
			if (cursorY_ > 0)
				cursorY_--;
			else if (startContainer_ > 0)
				startContainer_--;
			break;
		case GLFW_KEY_DOWN:
			if (cursorY_ < gridHeight_ - 1)
				cursorY_++;
			else if (startContainer_ < containers_.size())
				startContainer_++;
			break;
		default:
			break;
		}
}

void Client::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int posCursorY = 0;
    glm::vec2 drawPos(0.f, 0.f);
    glm::vec2 showSize = glm::vec2(width_ / gridWidth_, height_ / gridHeight_) * 0.8f;

    for (int c = startContainer_; c < containers_.size(); c++ ) {

    	if (drawPos.y < height_) {
    		text_->RenderText(containers_[c].name(), 0.f, drawPos.y, 1.0f);

    		drawPos.x = 0.f;
    		for (int s = containers_[c].startShow_; s < containers_[c].shows_.size(); s++) {
    			if (drawPos.x < width_) {
    				containers_[c].shows_[s].draw(tile_, drawPos + glm::vec2(20.f,30.f), showSize);
    			}
    			drawPos.x += width_ / gridWidth_;
    		}

    		drawPos.y += height_ / gridHeight_;
    	}
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
