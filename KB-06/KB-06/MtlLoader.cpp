#include "stdafx.h"
#include "MtlLoader.h"
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <vector>
#include "Material.h"
#include "StringHelper.h"
#include "LoggerPool.h"


std::map<std::string, Resource::Material> Resource::MtlLoader::Load(std::string file){
	std::ifstream ifs(file, std::ifstream::in);
	std::string line;
	std::vector<std::string> elements;
	std::map<std::string, Material> materials;

	std::string currentMaterial;
	while (ifs.good()) {
		getline(ifs, line);
		elements = Logger::StringHelper::split(line, ' ');
		if (elements.size() > 0){
			if (elements[0] == "newmtl"){
				currentMaterial = elements[1];
			}
			else if (elements[0] == "Ka"){
				materials[currentMaterial].ambientColor.r = strtof(elements[1].c_str(), 0);
				materials[currentMaterial].ambientColor.g = strtof(elements[2].c_str(), 0);
				materials[currentMaterial].ambientColor.b = strtof(elements[3].c_str(), 0);
			}
			else if (elements[0] == "Kd"){
				materials[currentMaterial].diffuseColor.r = strtof(elements[1].c_str(), 0);
				materials[currentMaterial].diffuseColor.g = strtof(elements[2].c_str(), 0);
				materials[currentMaterial].diffuseColor.b = strtof(elements[3].c_str(), 0);
			}
			else if (elements[0] == "Ks"){
				materials[currentMaterial].SpecularColor.r = strtof(elements[1].c_str(), 0);
				materials[currentMaterial].SpecularColor.g = strtof(elements[2].c_str(), 0);
				materials[currentMaterial].SpecularColor.b = strtof(elements[3].c_str(), 0);
			}
			else if (elements[0] == "Ns"){
				materials[currentMaterial].specularWeight = strtof(elements[1].c_str(), 0);
			}
			else if (elements[0] == "d" || elements[0] == "Tr"){
				materials[currentMaterial].alpha = strtof(elements[1].c_str(), 0);
			}
		}
	}
	ifs.close();
	Logger::Logger* logger = Logger::LoggerPool::GetInstance().GetLogger();
	logger->Log(Logger::Logger::DEBUG, "loaded: " + file);
	for (std::map<std::string, Material>::iterator iterator = materials.begin(); iterator != materials.end(); iterator++) {
		logger->Log(Logger::Logger::DEBUG, "Material name: " + iterator->first);
		logger->Log(Logger::Logger::DEBUG, "ambientColor: " +	std::to_string(iterator->second.ambientColor.r) + " " +		std::to_string(iterator->second.ambientColor.g) + " " +		std::to_string(iterator->second.ambientColor.b));
		logger->Log(Logger::Logger::DEBUG, "diffuseColor: " +	std::to_string(iterator->second.diffuseColor.r) + " " +		std::to_string(iterator->second.diffuseColor.g) + " " +		std::to_string(iterator->second.diffuseColor.b));
		logger->Log(Logger::Logger::DEBUG, "specularWeight: " + std::to_string(iterator->second.specularWeight));
	}
	Logger::LoggerPool::GetInstance().ReturnLogger(logger);
	return materials;
}