#include "Config.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ElectronCorrection
{
    Config::Config() : m_gasParams(), m_detParams(), m_outputDir("Invalid"), m_correctionFileName("Invalid")
    {
    }

    bool Config::Deserialize(const std::filesystem::path &path)
    {
        std::cout << "Loading configuration from " << path << "..." << std::endl;
        if (!std::filesystem::exists(path))
        {
            std::cout << "Config file " << path << " does not exist!" << std::endl;
            return false;
        }
        else if (path.extension() != ".json")
        {
            std::cout << "Config file " << path << " needs to have a .json extension (and be JSON data)" << std::endl;
            return false;
        }
        std::ifstream configFile(path);
        nlohmann::json jsonData = nlohmann::json::parse(configFile);

        m_outputDir = std::filesystem::path(jsonData["output_directory"]);
        if (!std::filesystem::exists(m_outputDir))
        {
            std::cout << "Creating output directory " << m_outputDir << std::endl;
            if (!std ::filesystem::create_directory(m_outputDir))
            {
                std::cout << "Could not create output directory!" << std::endl;
                return false;
            }
        }
        m_correctionFileName = jsonData["correction_file"];

        m_gasParams.name = jsonData["gas_name"];
        m_gasParams.pressure = jsonData["gas_pressure(Torr)"];
        m_gasParams.temperature = jsonData["gas_temperature(K)"];

        m_gasParams.eFieldMin = jsonData["electric_field_min(V/cm)"];
        m_gasParams.eFieldMax = jsonData["electric_field_max(V/cm)"];
        m_gasParams.eFieldSteps = jsonData["electric_field_steps"];
        m_gasParams.bField = jsonData["magnetic_field(T)"];

        m_detParams.cathodeVoltage = jsonData["cathode_voltage(V)"];
        m_detParams.anodeVoltage = jsonData["anode_voltage(V)"];
        m_detParams.firstRingVoltage = jsonData["first_ring_voltage(V)"];

        std::cout << "Configuration loaded succesfully!" << std::endl;
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