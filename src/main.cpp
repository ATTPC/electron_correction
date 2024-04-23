#include "Config.h"
#include "GenerateGas.h"
#include "GenerateCorrection.h"

#include <iostream>
#include <string>
#include <filesystem>

void PrintHelp()
{
    std::cout << "electron_correction is designed to generate correction files for the drift of electrons in the AT-TPC detector." << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "ecorr <optional_flags> <path_to_your_config.json>" << std::endl;
    std::cout << "Available optional flags:" << std::endl;
    std::cout << "--gen-gas: only generate the gas file specified by the configuration" << std::endl;
    std::cout << "--gen-corr: only generate the correction file specified by the configuration" << std::endl;
    std::cout << "--help: print this message" << std::endl;
    std::cout << "If no flags are passed, the program generates a gas file and a correction." << std::endl;
}

int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        PrintHelp();
        return 0;
    }
    else if (argc == 2)
    {
        if (std::string(argv[1]) == "--help")
        {
            PrintHelp();
            return 0;
        }
        std::filesystem::path configPath(argv[1]);
        ElectronCorrection::Config config;
        if (!config.Deserialize(configPath))
        {
            std::cout << "Could not load config. Quitting." << std::endl;
            return 0;
        }
        std::cout << "Generating gas file..." << std::endl;
        ElectronCorrection::GenerateGasFile(config.GetGasPath(), config.GetGasParameters(), config.GetFieldParameters());
        std::cout << "Generating correction file..." << std::endl;
        ElectronCorrection::GenerateElectronCorrection(config.GetCorrectionPath(), config.GetGasPath());
        std::cout << "Finished." << std::endl;
    }

    std::string flag = argv[1];
    std::filesystem::path configPath(argv[2]);
    if (flag == "--help")
    {
        PrintHelp();
        return 0;
    }

    ElectronCorrection::Config config;
    if (!config.Deserialize(configPath))
    {
        std::cout << "Could not load config. Quitting." << std::endl;
        return 0;
    }

    if (flag == "--gen-gas")
    {
        std::cout << "Generating gas file..." << std::endl;
        ElectronCorrection::GenerateGasFile(config.GetGasPath(), config.GetGasParameters(), config.GetFieldParameters());
        std::cout << "Finished." << std::endl;
    }
    else if (flag == "--gen-corr")
    {
        std::cout << "Generating correction file..." << std::endl;
        ElectronCorrection::GenerateElectronCorrection(config.GetCorrectionPath(), config.GetGasPath());
        std::cout << "Finished." << std::endl;
    }
    else
    {
        std::cout << "Unrecognized optional flag " << flag << std::endl;
    }
    return 0;
}