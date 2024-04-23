#pragma once

#include <string>
#include <filesystem>

namespace ElectronCorrection
{

    struct GasParameters
    {
        std::string name = "Invalid";
        double pressure = 0.0;
        double temperature = 0.0;
    };

    struct FieldParameters
    {
        double eFieldMin = 0.0;
        double eFieldMax = 0.0;
        int eFieldSteps = 0;
        double bField = 0.0;
    };

    class Config
    {
    public:
        Config();
        ~Config() = default;

        bool Deserialize(const std::filesystem::path &path);

        std::filesystem::path GetGasPath() const;
        std::filesystem::path GetCorrectionPath() const;

        const GasParameters &GetGasParameters() const { return m_gasParams; }
        const FieldParameters &GetFieldParameters() const { return m_fieldParams; }

    private:
        GasParameters m_gasParams;
        FieldParameters m_fieldParams;
        std::filesystem::path m_outputDir;
        std::string m_correctionFileName;

        static constexpr char s_invalidGas[] = "Invalid";
    };
}