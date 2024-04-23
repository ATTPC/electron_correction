#include "Config.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ElectronCorrection
{
    Config::Config() : m_gasParams(), m_fieldParams(), m_outputDir("Invalid"), m_correctionFileName("Invalid")
    {
    }

    bool Config::Deserialize(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            std::cout << "Config file " << path << " does not exist!" << std::endl;
            return false;
        }
        else if (path.extension() != "json")
        {
            std::cout << "Config file " << path << " needs to have a .json extension (and be JSON data)" << std::endl;
            return false;
        }
        std::ifstream configFile(path);
        nlohmann::json jsonData = nlohmann::json::parse(configFile);

        m_outputDir = std::filesystem::path(jsonData["output_directory"]);
        m_correctionFileName = jsonData["correction_file"];

        m_gasParams.name = jsonData["gas_name"];
        m_gasParams.pressure = jsonData["gas_pressure(Torr)"];
        m_gasParams.temperature = jsonData["gas_temperature(K)"];

        m_fieldParams.eFieldMin = jsonData["electric_field_min(V/cm)"];
        m_fieldParams.eFieldMax = jsonData["electric_field_max(V/cm)"];
        m_fieldParams.eFieldSteps = jsonData["electric_field_steps"];
        m_fieldParams.bField = jsonData["magnetic_field(T)"];

        return true;
    }

    std::filesystem::path Config::GetGasPath() const
    {
        if (m_gasParams.name == s_invalidGas or !std::filesystem::exists(m_outputDir))
        {
            std::cout << "Attempted Config::GetGasPath() with invalid parameters!" << std::endl;
            return std::filesystem::path("Invalid");
        }
        std::stringstream gasStr;
        gasStr << m_gasParams.name << "_" << int(m_gasParams.pressure) << "Torr_" << int(m_gasParams.temperature) << "K.txt";

        return m_outputDir / gasStr.str();
    }

    std::filesystem::path Config::GetCorrectionPath() const
    {
        if (!std::filesystem::exists(m_outputDir))
        {
            std::cout << "Attempted Config::GetCorrectionPath() with invalid parameters!" << std::endl;
            return std::filesystem::path("Invalid");
        }

        return m_outputDir / m_correctionFileName;
    }
}