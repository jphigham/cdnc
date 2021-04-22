#pragma once

#include <iostream>

#include <rapidjson/document.h>

static const char* kTypeNames[] =
    { "Null", "False", "True", "Object", "Array", "String", "Number" };

static void traverse(const rapidjson::Value &obj, std::size_t indent = 0)
{
	if (obj.IsObject()) {
		for (rapidjson::Value::ConstMemberIterator m = obj.MemberBegin(); m != obj.MemberEnd(); ++m) {
			for (std::size_t i = 0; i != indent; ++i)
				std::cout << " ";
			std::cout << "Type of member " << m->name.GetString() << " is " << kTypeNames[m->value.GetType()] << std::endl;
			const rapidjson::Value &child = obj[m->name.GetString()];
			if (child.IsArray()) {
				for (auto &m : child.GetArray())
					traverse(m, indent + 1);
			} else
				traverse(child, indent + 1);
		}
	}
}

class NodePrinter
{
public:

	static void printNode(const rapidjson::Value &node,
			size_t indent = 0,
			unsigned int level = 0,
			const std::string& nodeName = "");

	static void printObject(const rapidjson::Value &node,
			size_t indent = 0,
			unsigned int level = 0);

	static void printArray(const rapidjson::Value& node,
			size_t indent = 0,
			unsigned int level = 0);

	static std::string getIndentString(size_t indent = 0, unsigned int level = 0);

	static void printItems(const rapidjson::Value &items);

};
