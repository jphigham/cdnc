#include "parse_json_utils.h"

void NodePrinter::printNode(const rapidjson::Value &node,
		size_t indent,
		unsigned int level,
		const std::string& nodeName)
{
	std::cout << getIndentString(indent, level);

	if (!nodeName.empty())
		std::cout << nodeName << ": ";

	if (node.IsBool())
		std::cout << node.GetBool();

	else if (node.IsInt())
		std::cout << node.GetInt();

	else if (node.IsUint())
		std::cout << node.GetUint();

	else if (node.IsInt64())
		std::cout << node.GetInt64();

	else if (node.IsUint64())
		std::cout << node.GetUint64();

	else if (node.IsDouble())
		std::cout << node.GetDouble();

	else if (node.IsString())
		std::cout << node.GetString();

	else if (node.IsArray())
	{
		if (!nodeName.empty()) std::cout << "\n" << getIndentString(indent, level);
		printArray(node, indent, level);
	}

	else if (node.IsObject())
	{
		if (!nodeName.empty()) std::cout << "\n" << getIndentString(indent, level);
		printObject(node, indent, level);
	}

	std::cout << "\n";
}

void NodePrinter::printObject(const rapidjson::Value &node,
		size_t indent,
		unsigned int level)
{
	std::cout << "{\n";

	for (rapidjson::Value::ConstMemberIterator childNode = node.MemberBegin(); childNode != node.MemberEnd(); ++childNode)
	{
		printNode(childNode->value, indent, level + 1, childNode->name.GetString());
	}

	std::cout << getIndentString(indent, level) << "}";
}

void NodePrinter::printArray(const rapidjson::Value& node,
		size_t indent,
		unsigned int level)
{
	std::cout << "[\n";

	for (rapidjson::SizeType i = 0; i < node.Size(); ++i)
	{
		printNode(node[i], indent, level + 1);
	}

	std::cout << getIndentString(indent, level) << "]";
}

std::string NodePrinter::getIndentString(size_t indent, unsigned int level)
{
	return std::move(std::string(level * indent, ' '));
}

void NodePrinter::printItems(const rapidjson::Value &items)
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
