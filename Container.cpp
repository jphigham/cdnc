#include "Container.h"

#include <iostream>

Container::Container()
	: startShow_(0)
{

}

Container::~Container()
{
	shows_.clear();
}

void Container::get_json_shows(const rapidjson::Value &items)
{
	for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
		Show show;

		// title
		if (items[i]["text"]["title"]["full"].HasMember("series")) {
			show.setName(items[i]["text"]["title"]["full"]["series"]["default"]["content"].GetString());
			show.setMasterId(items[i]["image"]["tile"]["1.78"]["series"]["default"]["masterId"].GetString());
			show.setUrl(items[i]["image"]["tile"]["1.78"]["series"]["default"]["url"].GetString());
		} else if (items[i]["text"]["title"]["full"].HasMember("program")) {
			show.setName(items[i]["text"]["title"]["full"]["program"]["default"]["content"].GetString());
			show.setMasterId(items[i]["image"]["tile"]["1.78"]["program"]["default"]["masterId"].GetString());
			show.setUrl(items[i]["image"]["tile"]["1.78"]["program"]["default"]["url"].GetString());
		} else if (items[i]["text"]["title"]["full"].HasMember("collection")) {
			show.setName(items[i]["text"]["title"]["full"]["collection"]["default"]["content"].GetString());
			show.setMasterId(items[i]["image"]["tile"]["1.78"]["default"]["default"]["masterId"].GetString());
			show.setUrl(items[i]["image"]["tile"]["1.78"]["default"]["default"]["url"].GetString());
		} else {
			show.setName("UNKNOWN");
			show.setMasterId("UNKNOWN");
			show.setMasterId("UNKNOWN");
		}

		show.validate();
		shows_.push_back(show);
	}
}
