#include "GenerateCorrection.h"

#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/ComponentConstant.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/DriftLineRKF.hh"
#include "Garfield/TrackHeed.hh"

#include <iostream>
#include <fstream>

namespace ElectronCorrection
{
    // ATTPC Parameters
    // Number of rings
    static constexpr int s_nRings = 50;
    // Ring diameter [cm]
    static constexpr double s_ringDiameter = 0.47752;
    // Inner ring diameter [cm]
    static constexpr double s_innerRingDiameter = 56.1975;
    // Outer ring diameter [cm]
    static constexpr double s_outerRingDiameter = 62.23;
    // Distance between rings [cm]
    static constexpr double s_ringStep = 1.905;
    // Distance between flange and first ring [cm]
    static constexpr double s_firstRingDist = 4.7625;
    // Distance of Cathode plane [cm]
    static constexpr double s_cathodeDist = 100.33;
    // Distance of Anode plane [cm]
    static constexpr double s_anodeDist = 2.75;

    void GenerateElectronCorrection(const std::filesystem::path &outputPath, const std::filesystem::path &gasPath, const DetectorParameters &detectorParams)
    {
        if (!std::filesystem::exists(gasPath))
        {
            std::cout << "Gas file " << gasPath << " does not exist, cannot generate electron correction!" << std::endl;
            return;
        }

        std::cout << "Setting up AT-TPC field geometry..." << std::endl;

        Garfield::MediumMagboltz gas;
        gas.LoadGasFile(gasPath);

        Garfield::ComponentAnalyticField cage;
        cage.SetMedium(&gas);

        // Add rows of rings
        double volts, xcoord;
        for (int i = 0; i < s_nRings; i++)
        {
            volts = detectorParams.firstRingVoltage + i * (detectorParams.cathodeVoltage - detectorParams.firstRingVoltage) / ((double)s_nRings);
            xcoord = s_firstRingDist + i * s_ringStep;
            cage.AddWire(xcoord, s_outerRingDiameter / 2, s_ringDiameter, volts, "r");
            cage.AddWire(xcoord, -s_outerRingDiameter / 2, s_ringDiameter, volts, "r");
            cage.AddWire(xcoord, s_innerRingDiameter / 2, s_ringDiameter, volts, "r");
            cage.AddWire(xcoord, -s_innerRingDiameter / 2, s_ringDiameter, volts, "r");
        }

        // Add Cathode plane
        for (int i = -46; i <= 46; i++)
        {
            cage.AddWire(s_cathodeDist, i * 0.635, 0.634, detectorParams.cathodeVoltage, "c"); // use wires instead of plane
        }

        // Add Anode plane
        for (double x = -27.5; x <= 27.5; x += 0.5)
        {
            cage.AddWire(s_anodeDist, x, 0.499, detectorParams.anodeVoltage, "a"); // use wires instead of plane
        }
        cage.AddReadout("a");
        // Add the flange at ground potential
        cage.AddPlaneX(0., 0., "f");
        // Make it a cylinder around the X axis
        cage.EnableRotationSymmetryX(true);

        // Add a component for the solenoid
        Garfield::ComponentConstant solenoid;
        solenoid.SetMedium(&gas);
        solenoid.SetMagneticField(3.0, 0, 0);

        std::cout << "Geometry set. Calculating correction..." << std::endl;

        Garfield::Sensor sensor;
        sensor.AddComponent(&cage);
        sensor.AddComponent(&solenoid);
        sensor.EnableMagneticField(0, true);
        sensor.AddElectrode(&cage, "a");
        sensor.SetTimeWindow(0., 0.5, 100.);
        sensor.ClearSignal();

        Garfield::DriftLineRKF drift;
        drift.SetSensor(&sensor);

        double x_final, y_final, z_final, t_final;
        int st; // ?
        std::ofstream output(outputPath);
        for (double x_initial = 3.0; x_initial <= 100.0; x_initial += 1.0)
        {
            for (double y_initial = -27.0; y_initial <= 27.0; y_initial += 1.0)
            {
                drift.DriftElectron(x_initial, y_initial, 0., 0.);
                drift.GetEndPoint(x_final, y_final, z_final, t_final, st);
                output << x_initial << "\t"
                       << y_initial << "\t"
                       << x_final << "\t"
                       << y_final << "\t"
                       << z_final << "\t"
                       << t_final << "\n";
            }
            std::cout << "\rCalculated x=" << x_initial << " cm..." << std::flush;
        }
        std::cout << std::endl;
        std::cout << "Finished calculating. Output written to " << outputPath << std::endl;
    }
}
