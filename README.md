# electron_correction

Garfield calculations of the electron drift corrections due to the electric field distortion of the Active Target Time Projection Chamber (AT-TPC).

electron_correction is a cli application for generating correction files to be used in AT-TPC analysis (such as [Spyral](https://github.com/attpc/Spyral/))

## Required dependencies

electron_correction requires [Garfield++](https://gitlab.cern.ch/Garfield/garfieldpp) and [ROOT](https://root.cern.ch) to be installed on your machine, and to have the paths properly set (typically by sourcing `setupGarfield.sh` and `thisroot.sh` respectively).

electron_correction is built using C++17.

## Install

Download the repository from GitHub using `git clone --recursive https://github.com/attpc/electron_correction.git`. To build electron_correction use the following commands from within the repository:

```[bash]
mkdir build
cd build
cmake ..
make -j 4
```

If the program is built successfully it will be installed to the repository `bin` directory with the name `ecorr`.

## Usage

The interface is as shown below:

```[bash]
ecorr <optional_flags> <path_to_your_config.json>
```

The available flags are

- `--help`: generate a help message on the terminal. The program will not be run
- `--gen-gas`: only run the gas generation program.
- `--gen-corr`: only run the correction generation program, assuming the gas data has already been generated.

If no flags are passed, both the gas generation and correction programs are run. Generally speaking, the gas generation takes signifcantly longer, so gas files should be reused where possible.

### Configuration

Configurations are specified using JSON files. The format is as follows:

```[json]
{
    "output_directory": "/home/gordon/electron_correction/output/",
    "correction_file": "electrons_e20009.txt",
    "gas_name": "deuterium",
    "gas_pressure(Torr)": 600.0,
    "gas_temperature(K)": 293.15,
    "electric_field_min(V/cm)": 10.0,
    "electric_field_max(V/cm)": 1000.0,
    "electric_field_steps": 20,
    "magnetic_field(T)": 3.0,
    "cathode_voltage(V)": -60000.0,
    "anode_voltage(V)": -3020.0,
    "first_ring_voltage(V)": -3020.0
}
```

The above example is from the `e20009.json` example configuration shipped with the repository. The output directory will be created if it does not already exist.

## What does it do?

electron_correction is essentially a two step calculation. The first step is generating a gas file, which contains data about the gas and it's properties within a set of electromagnetic fields. This is generated using the MeduimMagboltz routines of Garfield++. The gas calculations are slow, and so the results of the gas calculation are saved to a file so they can be reused. 

The second step is the actual calculation of the correction to the drifting electrons. This is done by using the gas data of the previous step combined with the geometry and field settings of the AT-TPC for a specific experiment. The output of this program is saved to a file, and is compatible with the Spyral analysis framework.

It is recommended to, in general, first generate the gas file using the `--gen-gas` flag and then generate the correction using the `--gen-corr` flag. Then the gas file can be reused and you can avoid waiting for the gas calculation every time the correction is run for a given gas description.

