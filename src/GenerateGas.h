#pragma once

#include "Config.h"

namespace ElectronCorrection
{
    void GenerateGasFile(const std::filesystem::path &outputPath, const GasParameters &gasParams);
}