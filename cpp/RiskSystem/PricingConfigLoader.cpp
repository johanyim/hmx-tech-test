#include "PricingConfigLoader.h"
#include "RiskSystem/PricingEngineConfig.h"
#include "third-party/pugixml.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

PricingEngineConfig parseXml(const std::string& content)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(content.c_str());
    if (!result) {
        std::cerr << "Invalid XML: " << result.description() << '\n';
    }

    std::cout << "Managed to Parse" << "\n";

    return PricingEngineConfig();
}

std::string PricingConfigLoader::getConfigFile() const { return configFile_; }

void PricingConfigLoader::setConfigFile(const std::string& file)
{
    configFile_ = file;
}

PricingEngineConfig PricingConfigLoader::loadConfig()
{

    std::ifstream file(configFile_);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    std::string file_str = std::string((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    PricingEngineConfig config = parseXml(file_str);

    return config;
}
