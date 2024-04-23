#pragma once

#include <filesystem>

namespace ElectronCorrection
{
    void GenerateElectronCorrection(const std::filesystem::path &outputPath, const std::filesystem::path &gasPath);
}