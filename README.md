# electron_correction
Garfield calculations of the electron drift corrections due to the electric field distortions

This repository contains the source files used to run Garfield calculations of the electric field and electron drift in the AT-TPC.
The repository contains various folders, each folder corresponding to a particular experimental condition.
This is needed because of the different high voltage settings, as well as different gases and pressures used.
The file attpc.C contains the main code. The file gastable.C is used to calculate the gas table, which is then saved as a some_name.gas file.
This gas file is then loaded in attpc.C for faster calculations when trying various field configurations.

This code assumes the Garfield and Magboltz frameworks are already installed.
These programs used cmake to generate the Makefile from the CMakeLists.txt file.
Running the Makefile creates two executables "gastable" and "attpc".
