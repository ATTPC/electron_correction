#include <iostream>
#include <fstream>
#include <cstdlib>

#include <TCanvas.h>
#include <TROOT.h>
#include <TApplication.h>

#include "Garfield/ViewCell.hh"
#include "Garfield/ViewDrift.hh"
#include "Garfield/ViewSignal.hh"
#include "Garfield/ViewMedium.hh"
#include "Garfield/ViewField.hh"

#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/ComponentConstant.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/DriftLineRKF.hh"
#include "Garfield/TrackHeed.hh"

using namespace Garfield;

/*
bool readTransferFunction(Sensor& sensor) {

  std::ifstream infile;
  infile.open("mdt_elx_delta.txt", std::ios::in);
  if (!infile) {
    std::cerr << "Could not read delta response function.\n";
    return false;
  }
  std::vector<double> times;
  std::vector<double> values;
  while (!infile.eof()) {
    double t = 0., f = 0.;
    infile >> t >> f;
    if (infile.eof() || infile.fail()) break;
    times.push_back(1.e3 * t);
    values.push_back(f);
  }
  infile.close();
  sensor.SetTransferFunction(times, values);
  return true;
}
*/

int main(int argc, char * argv[]) {

  TApplication app("app", &argc, argv);
 
  // Make a gas medium.
  MediumMagboltz gas;
  
//	gas.SetComposition("hydrogen", 100.);
//	gas.SetPressure(300.);
//	gas.SetTemperature(293.15);
//	gas.SetFieldGrid(10., 1000., 20, true, 3., 3., 1, -0.1, 0.1, 5);
//	const int ncoll = 10;
//	gas.GenerateGasTable(ncoll);
//	gas.WriteGasFile("hydrogen_300.gas");

	gas.LoadGasFile("hydrogen_300.gas");
//  ViewMedium mediumView;
//  mediumView.SetMedium(&gas);
//  mediumView.PlotElectronVelocity('e');

//  auto installdir = std::getenv("GARFIELD_INSTALL");
//  if (!installdir) {
//    std::cerr << "GARFIELD_INSTALL variable not set.\n";
//    return 1;
//  }
//  const std::string path = installdir;
//  gas.LoadIonMobility(path + "/share/Garfield/Data/IonMobility_Ar+_Ar.txt");

  // Make a component for the field cage.
  ComponentAnalyticField cage;
  cage.SetMedium(&gas);
  // Number of rings
  const int nRing = 50;
  // Ring diameter [cm]
  const double dRing = 0.47752;
  // Inner ring diameter [cm]
  const double dInner = 56.1975;
  // Outer ring diameter [cm]
  const double dOuter = 62.23;
  // Distance between rings [cm]
  const double dStep = 1.905;
  // Distance between flange and first ring [cm]
  const double dFirst = 4.7625;
  // Distance of Cathode plane [cm]
  const double dCathode = 100.33;
  // Distance of Anode plane [cm]
  const double dAnode = 2.75;
  // Voltage on Cathode [V]
  const double vCathode = -45000;
  // Voltage on Anode [V]
  const double vAnode = -2070;
  // Voltage on first ring [v]
  const double vFirst = -2870;
  // Add rows of rings
  int i, j;
  double volts, xcoord;
  for (i=0; i<nRing; i++) {
    volts = vFirst + i*(vCathode-vFirst)/(double)nRing;
    xcoord = dFirst + i*dStep;
    cage.AddWire(xcoord, dOuter/2, dRing, volts, "r");
    cage.AddWire(xcoord, -dOuter/2, dRing, volts, "r");
    cage.AddWire(xcoord, dInner/2, dRing, volts, "r");
    cage.AddWire(xcoord, -dInner/2, dRing, volts, "r");
  }
  // Add Cathode plane
//  cage.AddPlaneX(dCathode, vCathode, "c");
  for (i=-46; i<=46; i++) {
    cage.AddWire(dCathode, i*0.635, 0.634, vCathode, "c"); // use wires instead of plane
  }
  // Add Anode plane
//  cage.AddPlaneX(dAnode, vAnode, "a");
  for (xcoord=-27.5; xcoord<=27.5; xcoord+=0.5) {
    cage.AddWire(dAnode, xcoord, 0.499, vAnode, "a"); // use wires instead of plane
  }
  cage.AddReadout("a");
  // Add the flange and outer shell at ground potential
  cage.AddPlaneX(0., 0., "f");
//  cage.AddPlaneY(-38.1, 0., "s");
//  cage.AddPlaneY(38.1, 0., "s");
  // Make it a cylinder around the X axis
  cage.EnableRotationSymmetryX(true);
  
//  FILE* ffile;
//  ffile = fopen("efield.txt", "w");
//  double xf, yf, zf, ex, ey, ez;
//  int status;
//  for (xf = 0; xf < 100; xf += 1) {
//  	for (yf = 0; yf < 30; yf += 1) {
//  		cage.ElectricField(xf, yf, 0, ex, ey, ez, (Medium*&)gas, status);
//  		fprintf(ffile, "%g\t%g\t%g\t%g\t%g\n", xf, yf, ex, ey, ez);
//  	}
//  }
//  fclose(ffile);
  
  // Make a component for the solenoid
  ComponentConstant sole;
  sole.SetMedium(&gas);
  sole.SetMagneticField(3, 0, 0);
  
	Sensor sensor;
	sensor.AddComponent(&cage);
	sensor.AddComponent(&sole);
	sensor.EnableMagneticField(0, true);
	sensor.AddElectrode(&cage, "a");
	sensor.SetTimeWindow(0., 0.5, 100.);
	sensor.ClearSignal();
  
    ViewField fieldView;
    fieldView.SetComponent(&cage);
    fieldView.SetPlaneXY();
    fieldView.SetArea(0., -32., 100., 32.);
    fieldView.SetVoltageRange(-60000., 0.);
    fieldView.SetNumberOfContours(100);
    TCanvas* cf = new TCanvas("cf", "", 1200, 600);
    cf->SetLeftMargin(0.16);
    fieldView.SetCanvas(cf);
    fieldView.PlotContour();
    
    TrackHeed track;
    track.SetParticle("proton");
    track.SetKineticEnergy(1.e8);
    track.SetSensor(&sensor);
//    track.EnableMagneticField();
    DriftLineRKF drift;
	drift.SetSensor(&sensor);

  	TCanvas* cv = new TCanvas("cv", "", 1200, 600);
  	TCanvas* cd = new TCanvas("cd", "", 1200, 600);
  	ViewCell cellView;
  	ViewDrift driftView;
  	cellView.SetCanvas(cv);
  	driftView.SetCanvas(cd);
  	cellView.SetComponent(&cage);
    drift.EnablePlotting(&driftView);
    track.EnablePlotting(&driftView);

//    track.NewTrack(90., 0, 0., 0., 0., 1., 0.);
//    double xc = 0., yc = 0., zc = 0., tc = 0., ec = 0., extra = 0.;
//    int nc = 0;
//    while (track.GetCluster(xc, yc, zc, tc, nc, ec, extra)) {
//      for (int k = 0; k < nc; ++k) {
//        double xe = 0., ye = 0., ze = 0., te = 0., ee = 0.;
//        double dx = 0., dy = 0., dz = 0.;
//        track.GetElectron(k, xe, ye, ze, te, ee, dx, dy, dz);
//        drift.DriftElectron(xe, ye, ze, te);
//      }
//    }

	FILE* efile;
	efile = fopen("electrons.txt", "w");
	double xe, ye, ze, te, xi, yi;
	int st;
	for (xi=3; xi<=100; xi+=1) {
		for (yi=-27; yi<=27; yi+=1) {
			drift.DriftElectron(xi, yi, 0., 0.);
			drift.GetEndPoint(xe, ye, ze, te, st);
			fprintf(efile, "%g\t%g\t%g\t%g\t%g\t%g\n", xi, yi, xe, ye, ze, te);
		}
		printf("calculated for x=%g cm\n", xi);
		fflush(stdout);
	}
	fclose(efile);


  	cellView.Plot2d();
  	driftView.Plot(false, true);


/*  
  // Wire radius [cm]
  const double rWire = 25.e-4;
  // Outer radius of the tube [cm]
  const double rTube = 0.71;
  // Voltages
  const double vWire = 2730.;
  const double vTube = 0.;
  // Add the wire in the centre.
  cmp.AddWire(0, 0, 2 * rWire, vWire, "s");
  // Add the tube.
  cmp.AddTube(rTube, vTube, 0, "t");
  // Request calculation of the weighting field. 
  cmp.AddReadout("s");

  // Make a sensor.
  Sensor sensor;
  sensor.AddComponent(&cmp);
  sensor.AddElectrode(&cmp, "s");
  // Set the signal time window.
  const double tstep = 0.5;
  const double tmin = -0.5 * tstep;
  const unsigned int nbins = 1000;
  sensor.SetTimeWindow(tmin, tstep, nbins);
  // Set the delta reponse function.
  if (!readTransferFunction(sensor)) return 0;
  sensor.ClearSignal();

  // Set up Heed.
  TrackHeed track;
  track.SetParticle("muon");
  track.SetEnergy(170.e9);
  track.SetSensor(&sensor);

  // RKF integration.
  DriftLineRKF drift;
  drift.SetSensor(&sensor);
  drift.SetGainFluctuationsPolya(0., 20000.);
  // drift.EnableIonTail();
 
  TCanvas* cD = nullptr;
  ViewCell cellView;
  ViewDrift driftView;
  constexpr bool plotDrift = true;
  if (plotDrift) {
    cD = new TCanvas("cD", "", 600, 600);
    cellView.SetCanvas(cD);
    cellView.SetComponent(&cmp);
    driftView.SetCanvas(cD);
    drift.EnablePlotting(&driftView);
    track.EnablePlotting(&driftView);
  }
 
  TCanvas* cS = nullptr;
  ViewSignal signalView;
  constexpr bool plotSignal = true;
  if (plotSignal) {
    cS = new TCanvas("cS", "", 600, 600);
    signalView.SetCanvas(cS);
    signalView.SetSensor(&sensor);
    signalView.SetLabelY("signal [fC]");
  } 

  const double rTrack = 0.3;
  const double x0 = rTrack;
  const double y0 = -sqrt(rTube * rTube - rTrack * rTrack);
  const unsigned int nTracks = 1;
  for (unsigned int j = 0; j < nTracks; ++j) {
    sensor.ClearSignal();
    track.NewTrack(x0, y0, 0, 0, 0, 1, 0);
    double xc = 0., yc = 0., zc = 0., tc = 0., ec = 0., extra = 0.;
    int nc = 0;
    while (track.GetCluster(xc, yc, zc, tc, nc, ec, extra)) {
      for (int k = 0; k < nc; ++k) {
        double xe = 0., ye = 0., ze = 0., te = 0., ee = 0.;
        double dx = 0., dy = 0., dz = 0.;
        track.GetElectron(k, xe, ye, ze, te, ee, dx, dy, dz);
        drift.DriftElectron(xe, ye, ze, te);
      }
    }
    if (plotDrift) {
      cD->Clear();
      cellView.Plot2d();
      constexpr bool twod = true;
      constexpr bool drawaxis = false;
      driftView.Plot(twod, drawaxis);
    }
    sensor.ConvoluteSignals();
    int nt = 0;
    if (!sensor.ComputeThresholdCrossings(-2., "s", nt)) continue;
    if (plotSignal) signalView.PlotSignal("s");
  }
*/
  app.Run(kTRUE);

}
