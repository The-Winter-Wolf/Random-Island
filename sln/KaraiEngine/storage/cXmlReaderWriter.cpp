#include "cXmlReaderWriter.h"

cXmlReaderWriter::cXmlReaderWriter(cConfigData* pConfiguration)
{
	this->pConfig = pConfiguration;
}

cXmlReaderWriter::~cXmlReaderWriter() {}

void cXmlReaderWriter::ReadConfiguration(const std::string& fullFileName)
{
	// Load data from file
	file<>* xmlReadFile = new file<>(fullFileName.c_str());
	xml_document<>* doc = new xml_document<>;
	doc->parse<0>(xmlReadFile->data());

	xml_node<>* firstNode = doc->first_node("Camera");
	if (firstNode != NULL)
	{
		m_StartNode = firstNode;

		GetValue("initialFoV", pConfig->camera.fovMinMax.x);
		
		GetValue("Position", "x", pConfig->camera.position.x);
		GetValue("Position", "y", pConfig->camera.position.y);
		GetValue("Position", "z", pConfig->camera.position.z);

		GetValue("Direction", "x", pConfig->camera.direction.x);
		GetValue("Direction", "y", pConfig->camera.direction.y);
		GetValue("Direction", "z", pConfig->camera.direction.z);
	}

	firstNode = doc->first_node("Lights");
	if (firstNode != NULL)
	{
		m_StartNode = firstNode->first_node("Light");
		while (m_StartNode != NULL)
		{
			GetValue("name", pConfig->light.name);
			GetValue("state", pConfig->light.prm2.x);

			GetValue("Param1", "type", pConfig->light.prm1.x);
			GetValue("Param1", "inAngle", pConfig->light.prm1.y);
			GetValue("Param1", "outAngle", pConfig->light.prm1.z);

			GetValue("Diffuse", "r", pConfig->light.diff.r);
			GetValue("Diffuse", "g", pConfig->light.diff.g);
			GetValue("Diffuse", "b", pConfig->light.diff.b);

			GetValue("Specular", "r", pConfig->light.spec.r);
			GetValue("Specular", "g", pConfig->light.spec.g);
			GetValue("Specular", "b", pConfig->light.spec.b);

			GetValue("Atten", "linear", pConfig->light.atten.y);
			GetValue("Atten", "quad", pConfig->light.atten.z);

			GetValue("Position", "x", pConfig->light.pos.x);
			GetValue("Position", "y", pConfig->light.pos.y);
			GetValue("Position", "z", pConfig->light.pos.z);

			GetValue("Direction", "x", pConfig->light.dir.x);
			GetValue("Direction", "y", pConfig->light.dir.y);
			GetValue("Direction", "z", pConfig->light.dir.z);

			pConfig->vLights.push_back(pConfig->light);

			m_StartNode = m_StartNode->next_sibling("Light");
		}
	}

	firstNode = doc->first_node("Models");
	if (firstNode != NULL)
	{
		m_StartNode = firstNode->first_node("Model");
		while (m_StartNode != NULL)
		{
			GetValue("fName", pConfig->object.friendlyName);
			GetValue("isVisible", pConfig->object.isVisible);
			GetValue("isLight", pConfig->object.hasLightColor.x);
			GetValue("isColor", pConfig->object.hasLightColor.y);
			GetValue("isTwoSided", pConfig->object.isSidedWireframe.x);
			GetValue("isWireframe", pConfig->object.isSidedWireframe.y);
			GetValue("flipUVs", pConfig->object.flipUVs);

			GetValue("FileName", "name", pConfig->object.fileName);
			GetValue("FilePath", "path", pConfig->object.filePath);

			GetValue("Position", "x", pConfig->object.position.x);
			GetValue("Position", "y", pConfig->object.position.y);
			GetValue("Position", "z", pConfig->object.position.z);

			GetValue("Rotation", "x", pConfig->object.rotation.x);
			GetValue("Rotation", "y", pConfig->object.rotation.y);
			GetValue("Rotation", "z", pConfig->object.rotation.z);

			GetValue("Scale", "x", pConfig->object.scale.x);
			GetValue("Scale", "y", pConfig->object.scale.y);
			GetValue("Scale", "z", pConfig->object.scale.z);

			GetValue("Color", "r", pConfig->object.color.r);
			GetValue("Color", "g", pConfig->object.color.g);
			GetValue("Color", "b", pConfig->object.color.b);
			GetValue("Color", "a", pConfig->object.color.a);

			GetValue("Feature", "hp", pConfig->object.hpXpSpeed.x);
			GetValue("Feature", "xp", pConfig->object.hpXpSpeed.y);
			GetValue("Feature", "speed", pConfig->object.hpXpSpeed.z);

			pConfig->vObjects.push_back(pConfig->object);

			m_StartNode = m_StartNode->next_sibling("Model");
		}
	}

	delete xmlReadFile;
	delete doc;

	return;
}

void cXmlReaderWriter::WriteConfiguration(const std::string& fullFileName)
{
	// Save data to file
	file<>* xmlWriteFile = new file<>(fullFileName.c_str());
	xml_document<>* doc = new xml_document<>;
	doc->parse <rapidxml::parse_full /*| rapidxml::parse_no_data_nodes*/>(xmlWriteFile->data());

	xml_node<>* confNode1 = doc->first_node();
	xml_node<>* confNode2 = confNode1->first_node("Camera");

	std::string tempPosX = std::to_string(pConfig->camera.position.x);
	confNode2->first_node("Position")->first_attribute("x")->value(tempPosX.c_str());
	std::string tempPosY = std::to_string(pConfig->camera.position.y);
	confNode2->first_node("Position")->first_attribute("y")->value(tempPosY.c_str());
	std::string tempPosZ = std::to_string(pConfig->camera.position.z);
	confNode2->first_node("Position")->first_attribute("z")->value(tempPosZ.c_str());

	std::ofstream file;
	file.open(fullFileName);
	file << *doc;
	file.close();

	delete xmlWriteFile;
	delete doc;

	return;
}

bool cXmlReaderWriter::GetValue(const std::string& attrName, int& valueOut)
{
	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = m_StartNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}
	
	std::string tempStr = tempAttr->value();
	if (tempStr.size() == 0)
	{
		return false;
	}

	valueOut = std::stoi(tempStr);

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& attrName, bool& valueOut)
{
	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = m_StartNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	std::string tempStr = tempAttr->value();
	if (tempStr == "1" || tempStr == "1.0" || tempStr == "true")
	{
		valueOut = true;
	}
	else
	{
		valueOut = false;
	}

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& attrName, float& valueOut)
{
	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = m_StartNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	std::string tempStr = tempAttr->value();
	if (tempStr.size() == 0)
	{
		return false;
	}

	valueOut = std::stof(tempStr);

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& attrName, std::string& valueOut)
{
	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = m_StartNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	valueOut = tempAttr->value();

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& nodeName, const std::string& attrName, int& valueOut)
{
	if (nodeName.size() == 0)
	{
		return false;
	}

	xml_node<>* tempNode = m_StartNode->first_node(nodeName.c_str());
	if (!tempNode)
	{
		return false;
	}

	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = tempNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	std::string tempStr = tempAttr->value();
	if (tempStr.size() == 0)
	{
		return false;
	}

	valueOut = std::stoi(tempStr);

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& nodeName, const std::string& attrName, bool& valueOut)
{
	if (nodeName.size() == 0)
	{
		return false;
	}

	xml_node<>* tempNode = m_StartNode->first_node(nodeName.c_str());
	if (!tempNode)
	{
		return false;
	}

	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = tempNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	std::string tempStr = tempAttr->value();
	if (tempStr == "1" || tempStr == "1.0" || tempStr == "true")
	{
		valueOut = true;
	}
	else
	{
		valueOut = false;
	}

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& nodeName, const std::string& attrName, float& valueOut)
{
	if (nodeName.size() == 0)
	{
		return false;
	}

	xml_node<>* tempNode = m_StartNode->first_node(nodeName.c_str());
	if (!tempNode)
	{
		return false;
	}

	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = tempNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	std::string tempStr = tempAttr->value();
	if (tempStr.size() == 0)
	{
		return false;
	}

	valueOut = std::stof(tempStr);

	return true;
}

bool cXmlReaderWriter::GetValue(const std::string& nodeName, const std::string& attrName, std::string& valueOut)
{
	if (nodeName.size() == 0)
	{
		return false;
	}

	xml_node<>* tempNode = m_StartNode->first_node(nodeName.c_str());
	if (!tempNode)
	{
		return false;
	}

	if (attrName.size() == 0)
	{
		return false;
	}

	xml_attribute<>* tempAttr = tempNode->first_attribute(attrName.c_str());
	if (!tempAttr)
	{
		return false;
	}

	std::string tempStr = tempAttr->value();
	if (tempStr.size() == 0)
	{
		return false;
	}

	valueOut = tempAttr->value();

	return true;
}

//void cXmlReaderWriter::WriteConfiguration(sConfig* pConfig)
//{
//	// Load UnitPrefs File
//	file<>* xmlFile = new file<>(configXML.c_str());
//	xml_document<>* doc = new xml_document<>;
//	doc->parse <parse_default>(xmlFile->data());
//
//	xml_node<>* confNode1 = doc->first_node()->first_node("Camera");
//
//	std::string tempStr;
//	tempStr = std::to_string(pConfig->camera.pos.x);
//	confNode1->first_node("Position")->first_attribute("x")->value(tempStr.c_str());
//	tempStr = std::to_string(pConfig->camera.pos.y);
//	confNode1->first_node("Position")->first_attribute("y")->value(tempStr.c_str());
//	tempStr = std::to_string(pConfig->camera.pos.z);
//	confNode1->first_node("Position")->first_attribute("z")->value(tempStr.c_str());
//
//	tempStr = std::to_string(pConfig->camera.dir.x);
//	confNode1->first_node("Direction")->first_attribute("x")->value(tempStr.c_str());
//	tempStr = std::to_string(pConfig->camera.dir.y);
//	confNode1->first_node("Direction")->first_attribute("y")->value(tempStr.c_str());
//	tempStr = std::to_string(pConfig->camera.dir.z);
//	confNode1->first_node("Direction")->first_attribute("z")->value(tempStr.c_str());
//
//	xml_node<>* confNode2 = confNode1->next_sibling("Lights");
//	xml_node<>* confNode3 = confNode2->first_node("Light");
//
//	for (sLight light : pConfig->vLights)
//	{
//		confNode3->first_attribute("name")->value(light.name.c_str());
//
//		tempStr = std::to_string(light.prm2.x);
//		confNode3->first_attribute("state")->value(tempStr.c_str());
//
//		tempStr = std::to_string(light.prm1.x);
//		confNode3->first_node("Param1")->first_attribute("type")->value(tempStr.c_str());
//		tempStr = std::to_string(light.prm1.y);
//		confNode3->first_node("Param1")->first_attribute("inAngle")->value(tempStr.c_str());
//		tempStr = std::to_string(light.prm1.z);
//		confNode3->first_node("Param1")->first_attribute("outAngle")->value(tempStr.c_str());
//
//		tempStr = std::to_string(light.diff.r);
//		confNode3->first_node("Diffuse")->first_attribute("r")->value(tempStr.c_str());
//		tempStr = std::to_string(light.diff.g);
//		confNode3->first_node("Diffuse")->first_attribute("g")->value(tempStr.c_str());
//		tempStr = std::to_string(light.diff.b);
//		confNode3->first_node("Diffuse")->first_attribute("b")->value(tempStr.c_str());
//
//		tempStr = std::to_string(light.spec.r);
//		confNode3->first_node("Specular")->first_attribute("r")->value(tempStr.c_str());
//		tempStr = std::to_string(light.spec.g);
//		confNode3->first_node("Specular")->first_attribute("g")->value(tempStr.c_str());
//		tempStr = std::to_string(light.spec.b);
//		confNode3->first_node("Specular")->first_attribute("b")->value(tempStr.c_str());
//
//		tempStr = std::to_string(light.atten.y);
//		confNode3->first_node("Atten")->first_attribute("linear")->value(tempStr.c_str());
//		tempStr = std::to_string(light.atten.z);
//		confNode3->first_node("Atten")->first_attribute("quad")->value(tempStr.c_str());
//
//		tempStr = std::to_string(light.pos.x);
//		confNode3->first_node("Position")->first_attribute("x")->value(tempStr.c_str());
//		tempStr = std::to_string(light.pos.y);
//		confNode3->first_node("Position")->first_attribute("y")->value(tempStr.c_str());
//		tempStr = std::to_string(light.pos.z);
//		confNode3->first_node("Position")->first_attribute("z")->value(tempStr.c_str());
//
//		tempStr = std::to_string(light.dir.x);
//		confNode3->first_node("Direction")->first_attribute("x")->value(tempStr.c_str());
//		tempStr = std::to_string(light.dir.y);
//		confNode3->first_node("Direction")->first_attribute("y")->value(tempStr.c_str());
//		tempStr = std::to_string(light.dir.z);
//		confNode3->first_node("Direction")->first_attribute("z")->value(tempStr.c_str());
//
//		if (confNode3->next_sibling("Light") == NULL)
//		{
//			xml_node<>* cloneNode = doc->clone_node(confNode2->first_node("Light"));
//			confNode2->append_node(cloneNode);
//			confNode3 = confNode3->next_sibling("Light");
//		}
//		else
//		{
//			confNode3 = confNode3->next_sibling("Light");
//		}
//	}
//
//	//xml_node<>* confNode4 = confNode2->next_sibling("Models");
//
//	//xml_node<>* confNode5 = confNode4->first_node("Model");
//
//	//for(sModel model : pConfig->vModels)
//	//{
//	//	confNode5->first_attribute("fName")->value(model.friendName.c_str());
//
//	//	tempStr = std::to_string(model.hasLight);
//	//	confNode5->first_attribute("isLight")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.isVisible);
//	//	confNode5->first_attribute("isVisible")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.isWireframe);
//	//	confNode5->first_attribute("isWireframe")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.hasDebugColor);
//	//	confNode5->first_attribute("isColor")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.flipUVs);
//	//	confNode5->first_attribute("flipUVs")->value(tempStr.c_str());
//
//	//	confNode5->first_node("FileName")->first_attribute("name")->value(model.fileName.c_str());
//	//	confNode5->first_node("FilePath")->first_attribute("path")->value(model.filePath.c_str());
//
//	//	tempStr = std::to_string(model.pos.x);
//	//	confNode5->first_node("Position")->first_attribute("x")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.pos.y);
//	//	confNode5->first_node("Position")->first_attribute("y")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.pos.z);
//	//	confNode5->first_node("Position")->first_attribute("z")->value(tempStr.c_str());
//
//	//	tempStr = std::to_string(model.rot.x);
//	//	confNode5->first_node("Rotation")->first_attribute("x")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.rot.y);
//	//	confNode5->first_node("Rotation")->first_attribute("y")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.rot.z);
//	//	confNode5->first_node("Rotation")->first_attribute("z")->value(tempStr.c_str());
//
//	//	tempStr = std::to_string(model.scale.x);
//	//	confNode5->first_node("Scale")->first_attribute("x")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.scale.y);
//	//	confNode5->first_node("Scale")->first_attribute("y")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.scale.z);
//	//	confNode5->first_node("Scale")->first_attribute("z")->value(tempStr.c_str());
//
//	//	tempStr = std::to_string(model.color.r);
//	//	confNode5->first_node("Color")->first_attribute("r")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.color.g);
//	//	confNode5->first_node("Color")->first_attribute("g")->value(tempStr.c_str());
//	//	tempStr = std::to_string(model.color.b);
//	//	confNode5->first_node("Color")->first_attribute("b")->value(tempStr.c_str());
//
//	//	if (confNode5->next_sibling("Model") == NULL)
//	//	{
//	//		xml_node<>* cloneNode = doc->clone_node(confNode4->first_node("Model"));
//	//		confNode4->append_node(cloneNode);
//	//		confNode5 = confNode5->next_sibling("Model");
//	//	}
//	//	else
//	//	{
//	//		confNode5 = confNode5->next_sibling("Model");
//	//	}
//	//}
//
//	std::ofstream file;
//	file.open(configXML);
//	file << *doc;
//	file.close();
//
//	delete xmlFile;
//	delete doc;
//
//	return;
//}