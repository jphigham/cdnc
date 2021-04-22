#include "Client.h"

#include "data_api.h"
#include "resource_path.h"

#include <fstream>
#include <iostream>

#include <rapidjson/error/en.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Curl.h"
#include "parse_json_utils.h"

Client::Client()
{

}

Client::~Client()
{

}

void Client::init()
{
	auto home_json_data = std::make_unique<std::string>();
	Curl::fetch(home_json_url.c_str(), home_json_data);
	parse_json(home_json_data);
}

void Client::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

void Client::get_json_items(const rapidjson::Value &items)
{
	for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
		if (items[i]["text"]["title"]["full"].HasMember("series"))
			std::cout << " [s] " << items[i]["text"]["title"]["full"]["series"]["default"]["content"].GetString() << std::endl;
		else if (items[i]["text"]["title"]["full"].HasMember("program"))
			std::cout << " [p] " << items[i]["text"]["title"]["full"]["program"]["default"]["content"].GetString() << std::endl;
		else if (items[i]["text"]["title"]["full"].HasMember("collection"))
			std::cout << " [c] " << items[i]["text"]["title"]["full"]["collection"]["default"]["content"].GetString() << std::endl;
		else
			std::cout << " [?] " << std::endl;
	}
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
		std::cout << "Found " << containers.Size() << " containers" << std::endl;

		for (rapidjson::SizeType i = 0; i < containers.Size(); ++i) {

			const rapidjson::Value &container = containers[i];
			std::cout << container["set"]["text"]["title"]["full"]["set"]["default"]["content"].GetString() << std::endl;

			if (container["set"].HasMember("refId")) {
				const rapidjson::Value &refId = containers[i]["set"]["refId"];
				std::cout << " : " << refId.GetString() << std::endl;

				// Read or fetch ref json
				auto ref_json_data = std::make_unique<std::string>();

				std::filesystem::path ref_json_path = json_path("set-refId-");
				ref_json_path += refId.GetString();
				ref_json_path += ".json";

				if (std::filesystem::exists(ref_json_path)) {
					std::cout << "Reading " << ref_json_path << std::endl;
					if (read_json(ref_json_path.string().c_str(), ref_json_data))
						std::cerr << "problem reading" << std::endl;
				} else {
					std::string ref_json_url;
					ref_json_url = ref_json_base_url;
					ref_json_url += refId.GetString();
					ref_json_url += ".json";
					std::cout << "Fetching " << ref_json_url << std::endl;
					if (Curl::fetch(ref_json_url.c_str(), ref_json_data))
						std::cerr << "problem fetching" << std::endl;
				}

				rapidjson::Document ref_doc;
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

					std::cout << " - Found " << ref_items.Size() << " items" << std::endl;
					NodePrinter::printItems(ref_items);
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
				std::cout << " - Found " << items.Size() << " items" << std::endl;
				NodePrinter::printItems(items);
			}
		}
	}
}
