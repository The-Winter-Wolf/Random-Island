#pragma once
#include "cConfigData.h"

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_iterators.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <rapidxml/rapidxml_utils.hpp>

using namespace rapidxml;

class cXmlReaderWriter
{
public:
	cXmlReaderWriter(cConfigData* pConfiguration);
	~cXmlReaderWriter();

	void ReadConfiguration(const std::string& fullFileName);

	void WriteConfiguration(const std::string& fullFileName);

private:

	cConfigData* pConfig;
	xml_node<>* m_StartNode;

	bool GetValue(const std::string& attrName, int& valueOut);
	bool GetValue(const std::string& attrName, bool& valueOut);
	bool GetValue(const std::string& attrName, float& valueOut);
	bool GetValue(const std::string& attrName, std::string& valueOut);

	bool GetValue(const std::string& nodeName, const std::string& attrName, int& valueOut);
	bool GetValue(const std::string& nodeName, const std::string& attrName, bool& valueOut);
	bool GetValue(const std::string& nodeName, const std::string& attrName, float& valueOut);
	bool GetValue(const std::string& nodeName, const std::string& attrName, std::string& valueOut);
};