#include "GenerateGas.h"

#include "Garfield/MediumMagboltz.hh"

#include <iostream>

namespace ElectronCorrection
{
    void GenerateGasFile(const std::filesystem::path &outputPath, const GasParameters &gasParams)
    {
        std::cout << "Generating gas file " << outputPath << "..." << std::endl;
        if (std::filesystem::exists(outputPath))
        {
            std::cout << "WARNING: Gas file " << outputPath << " already exists! We're gonna overwrite it!" << std::endl;
        }

        Garfield::MediumMagboltz gas;
        gas.SetComposition(gasParams.name, 100.);
        gas.SetPressure(gasParams.pressure);
        gas.SetTemperature(gasParams.temperature);
        gas.SetFieldGrid(gasParams.eFieldMin, gasParams.eFieldMax, gasParams.eFieldSteps, true, gasParams.bField, gasParams.bField, 1, 0.0, 0.0, 1);
        const int ncoll = 10;
        gas.GenerateGasTable(ncoll);
        gas.WriteGasFile(outputPath);

        std::cout << "Gas file " << outputPath << " successfully generated." << std::endl;
    }
}