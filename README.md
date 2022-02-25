# Glass Prototype Simulation

Repository containing the code for simulation of the energy deposition profiles and energy resolution in the Glass Prototype NPS experiment. Table of contents:

- [Developer notes](#developer-notes)
    - [Recommended OS for software development](#recommended-os-for-software-development)
    - [Installing CERN ROOT and Geant4](#installing-cern-root-and-geant4-on-personal-computer)
    - [Setting up the Glass Prototype Project in Eclipse](#setting-up-the-glass-prototype-project-in-eclipse)
- [User notes](#user-notes)
    - [Logging to the Computing Farm](#logging-to-the-computing-farm)
    - [Compilation of the Executable Program](#compilation-of-the-executable-program)
    - [Analysis of the Output Files](#analysis-of-the-outputfiles)
    - [Analysis of a Single File](#analysis-of-a-single-file)
    - [Plotting Energy Resolution for Multiple Energies](#plotting-energy-resolution-for-multiple-energies)
    - [Copying the Results to Local Computer](#copying-the-results-to-local-computer)
    - [Notes from the Former Developers](#notes-from-the-former-developers)

## Developer notes

In this section we document useful information for application developers, namely: how to install ROOT the Geant4 on local computer and how to setup the project in Integrated Desktop Environment (IDE).

### Recommended OS for software development

Red Hat Enterprise Linux (RHEL) is a gold standard in linux family operating systems for commercial market. RHEL is open-source distribution that comes with paid support. Within the past decade scinetific community (organizations like CERN or Fermilab and JLab) were utilizing a free RHEL clone named CentOS. CentOS is originally a RHEL fork, a community driven project that established good relationships with Red Hat headquaters and even received funding and support since 2014. 

CentOS is currently installed on the JLab's computung farm nodes. Computing farm nodes are rich with CPU and memory resources and meant to be used for running final calculations that process large amounts of data. However initial program development and debugging often needs to be carried out on a personal computer.

Red Hat provides a free operating system named Fedora that is based on RHEL but oriented more for personal computers rather than enterprise. Red Hat utilizes Fedora to test the bleeding edge technologies that later might be merged into RHEL. Fedora has the same package manager (RPM) as the RHEL and CentOS. Also Fedora uses the same package manager (YUM, later DNF).

In my point of view, similarities between RHEL, CentOS and Fedora strongly lead towards selecting Fedora as a distribution of choice for the software development of the ROOT-based programs for scientific application. However, any linux distribution should work just fine for development of a C++ ROOT project in Eclipse.

### Installing CERN ROOT and Geant4 on personal computer

Scripts that install most recent CERN ROOT and Geant4 versions on Fedora linux [can be found here](https://github.com/petrstepanov/fedora-scripts). Scripts builds ROOT with debug symbols and all options turned on. Geant4 is built with debug symbols and most common options. Installation instructions are following:
```
cd ~/Downloads
git clone https://github.com/petrstepanov/fedora-scripts && cd ~/Downloads
chmod +x install-root-latest.sh
sh ./install-root-latest.sh
chmod +x install-geant4-latest.sh
sh ./install-geant4-latest.sh
```

Scripts locate the latest ROOT and Geant versions on the CERN servers, download the framework source code to corresponding `~/Source/` folder, satisfyes the dependencies, build the frameworks, installs binaries in `~/Applications/` folder and sets up the environment variables.

Feel free to reach out to [Petr Stepanov](https://github.com/petrstepanov/) with respect to the scripts issues. Find more information about compilation of CERN ROOT from source [here](https://root.cern/install/build_from_source/). Geant4 build documentation can be found [here](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/installguide.html).

### Setting up the Glass Prototype Project in Eclipse

First, install Eclipse IDE. This process is documented in [Chapter 6](https://petrstepanov.com/static/petr-stepanov-dissertation-latest.pdf). Make sure to increase Eclipse heap and indexer cache limits because CERN ROOT is a rather resourceful framework.

After installation go to Window -> Preferences -> General -> Workspace. Uncheck "Refresh on access". Otherwise Eclipse may randomly start refreshing the workspace. For for external (non-CDT managed) build tools Eclipse does not provide folder specific Refresh Policy settings. Therefore on a workspace refresh Eclipse will index all project source files including ROOT sources. This takes quite a few time and CPU cycles. Therefore we will try to avoid workspace refresh operations as much as possible.

Next, check out the Git repository into the desired location on your computer. I usually keep most of the Git repositories in `~/Development` folder.
```
mkdir -p ~/Development && cd ~/Development
git clone https://github.com/JeffersonLab/glass-prototype
```

Next we need to set up an Eclipse project. Thankfully, CMake has an automated way of generating the Eclipse project. The CMake generator command is `cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../project/path`, where `../project/path` path must be relative or absolute path of the project source folder that contains CMake cache file `CMakeLists.txt`. Generally speaking there are two options:
1. **In-source build**. Eclipse project folder files are located the source program folder next to the `CMakeLists.txt` file. It is not favorable because project files will interfere with the Git tree and multiple excludes in `.gitignore` will be required. Also I've experienced Eclipse indexer issued using this method.
2. **Out-of-source build**. Eclipse project is located outside of the Git repository. This option is preferrable because the project, build and executable files are separated trom the Git project tree. We will use this option.

```
mkdir glass-prototype-project && cd glass-prototype-project
cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../glass-prototype/
```

More information regarding the CMake Eclipse generator can be found: [on Mantid project page](https://www.mantidproject.org/Setting_up_Eclipse_projects_with_CMake), [on Javier V. Gómez website](https://jvgomez.github.io/pages/how-to-configure-a-cc-project-with-eclipse-and-cmake.html), [on CMake Wiki](https://gitlab.kitware.com/cmake/community/-/wikis/home), [again on CMake Wiki](https://gitlab.kitware.com/cmake/community/-/wikis/doc/editors/Eclipse-CDT4-Generator) and [again on CMake Wiki](https://gitlab.kitware.com/cmake/community/-/wikis/doc/editors/Eclipse-UNIX-Tutorial).

#### Adding ROOT and Geant Sources to Eclipse Project

By default, CMake project generator creates Eclipse project and links ROOT includes (header files) to the project. However, **extensive debugging requires access to Geant and ROOT source files**. An effective way of linking ROOT framework source files to Eclipse project is following:
* Download amd extract ROOT and Geant source packages to local computer. I usually keep all sources under `~/Source/` folder.
* Optional: unpack and store ROOT sources on the RAMDISK (hard drive in memory) to improve indexing speed on older SATA3 and pre-nvme SSD hard drives.
* Symlink all ROOT and Geant4 folders under the project's source folder. Tip: it is better if sources are arranged without nested directory structure. This greatly improves Eclipse indexing speed.

I wrote a [special Makefile function](https://stackoverflow.com/a/69463832) that performs above functuionality. In order to make it work, pass locations of the ROOT and GEANT4 sources to `CMakeLists.txt` as CMake variables:

```
cmake -G"Eclipse CDT4 - Unix Makefiles" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DROOT_SOURCE_LOCATION="$HOME/ramdisk-persistent/current/root*/" \
      -DGEANT4_SOURCE_LOCATION="$HOME/ramdisk-persistent/current/geant4*/source/" \
      -DCMAKE_ECLIPSE_GENERATE_LINKED_RESOURCES=OFF \
      ../glass-prototype
```
In the command above the ROOT library path need to be replaced with one on your file system. Here `-DCMAKE_ECLIPSE_GENERATE_LINKED_RESOURCES=OFF` [disables creation](https://gitlab.kitware.com/cmake/cmake/-/issues/19107) of the `[Subprojects]` folder for out-of source Cmake build. Presence of this folder duplicates all project source files and slows down the indexer.

Finaly open Eclipse and go to File > Open Projects from File System... Specify the project location in the modal dialog by clicking the "Directory..." button. Locate the `~/Development/glass-prototype-project` project folder. Click "Finish". 

Tip: Eclipse will automatically start indexing the project. Please kill this process because Eclipse will refresh the workspace and re-index all project again after the first launch of the Debug configuration.

#### Setting Eclipse Debug and Run Configurations

We will start from setting up the main Debug configuration for Geant4 `glass` executable. Expand 🎯 Build Targets under the main project node in the Project Explorer window. Double-click "All" to compile the executable.

1. In menu "Run" open "Debug configurations". Right click "C/C++ Application" and select "New Configuration".
2. Click "Browse" next to the "Project" field. Select "glass-Debug@glass-prototype-project"
3. Click "Search Project" next to the "C/C++ Application" field. Specify the "glass" execuable built earlier.
4. Select "Enable Auto Build"
5. Go to the "Debugger" tab. Uncheck "Stop on startup at:".

Program source code contains some ROOT scripts under the `./draw/` folder. These scripts are designed for plotting and analysing of the output data. Run configurations for the above scripts are needed to be set up in the Eclipse IDE.
Follow the below steps to create run configurations for every ROOT script under the `./draw/folder`:

1. In menu "Run" open "Run Configurations".  Right click "C/C++ Application" and select "New Configuration".
2. In the "Name" field enter the name of the ROOT script you want to create the configuration for, e.g. `scintTransDet` (or `detectors`, `materials`)
3. Click "Browse" button next to the "C/C++ Application" field. Locate your CERN "root" executable file (run `which root` in Terminal).
4. Select "Enable auto build". This is importnat because before running the root scripts they need to be copied to the `./eclipse` folder. `CMakeLists.txt` takes care of that.
5. Go to "Arguments" tab. Specify the name of the ROOT script from the `./draw` folder to be launched, e.g. `./draw/scintTransDet.cpp`.

Repeat the above process for any script you would like to run in the Eclipse. Eclipse configuration outlined above contains ROOT source files and headers. Therefore code navigation and autocompletion for the ROOT scripts does work.

Finally we can run the project in Debug mode. In Eclipse menu select `Run > Debug`. Eclipse will run the project and simultaneously start indexing all ROOT source files. Depending on the speed of your hard drive and memory indexing will require from several minutes to about an hour. On older computers with SATA hard drives I recommend storing Eclipse workspace folder as well as ROOT and Geant4 sources [on the RAMDISK](https://github.com/patrikx3/ramdisk).

#### Eclipse Post-Install Notes

Geant4 follows [2 character code indentation](https://geant4.web.cern.ch/collaboration/coding_style_guidelines_motivations). In Eclipse menu go to Window > Preferences > C/C++ > Code Style > Formatter. Create a new Formatter profile from the default "K&R" and Specify following:
- Tab policy: Spaces only.
- Tab size: 2.

CMake links program source folder to the project twice: as `[Source directory]` and `[Subprojects]`. In order to avoid confusion we will exclude duplicated files. Press `CTRL+Shift+R` to show the "Open Resource" dialog. Click on three dots on the right. Check "Filter Duplicated Resources".

To enable line numbers in Eclipse, go to Window > Preferences > General > Editors > Text Editors. Check the "Show line numbers" option.

It is useful to have Eclipse "Save resources automatically before build" in Window > Preferences > General > Workspace. from the list.

## User notes

This section is designed for the end users and describes the process of running the simulation and obtaining the experimental results on the Computing Farm. There is no need to install the Geant4 toolkit on the farm. It comes preinstalled. 

### Logging to the Computing Farm

Program code supports the running the executable in the Interactive simulation mode. However in order to forward the graphical output from the Computing Farm via the ssh protocol, it is necessary to pass the `-Y` parameter to the `ssh` command:
```
ssh -Y <your-username>@login.jalb.org
ssh ifarm
```
In order to run the Geant4 in multithreading mode on the Computational Farm, it is necessary to specify Multi-Threading (`MT`) suffix for the the `GEANT4_VERSION` environment variable. This step is optional. Plese note that is considered impolite to "grab" large numbers of CPU cores or run long jobs on the interactive nodes. Testing is OK, but make the tests short.
```
set GEANT4_VERSION=4.10.06.p02MT
```

After logging in to the Computing Farm it is necessary to source the latest environment or the Geant4 and ROOT toolkits. In order to see the available versions of the environment run the following command:
```
csh /site/12gev_phys/softenv.csh
```

The output of the above command will provide the avalable versions of the software environments. It is recommended to use the latest environment unless the software requires some specific version of it. Currently (Dec 5, 2020) the latest version of the environment is `2.4`. Therefore one will execute:
```
source /site/12gev_phys/softenv.csh 2.4
```

The shortcut to source always the latest possible environment is following (grep's regexp lookahead):
```
source /site/12gev_phys/softenv.csh `grep -oP "\d\.\d(?= \(prod)" /site/12gev_phys/softenv.csh`
```

### Compilation of the Executable Program

Next we check out this GitHub repository under the `Downloads` folder:
```
mkdir -p ~/Downloads && cd ~/Downloads/
rm -rf ./glass-prototype
git clone https://github.com/JeffersonLab/glass-prototype
```

In order to not interfere with the original program code we will create a dedicated build directory outside of the Git repository tree.
```
mkdir -p ./glass-prototype-bulid && cd ./glass-prototype-bulid
cmake -DGeant4_DIR=$G4LIB/Geant4-$G4DATA_VERSION/ ./../glass-prototype/
make -j`nproc`
```


### Running the Simulation

Now the program is compiled and ready to launch. However, we need to specify the detector geometry, incident particle properties and other minor in the macro file.
```
nano ./macros/farm.mac
```

Adjust values of the following macro commands, that correspond to a variety of simulation properties:

```
# Crystal size along X, Y and Z axis
/detector/setCrystalSize 20 20 200 mm
# Crystal material - currently supported values are BaGdSiO, BaSi2O5, PbWO4
/detector/setCrystalMaterial BaGdSiO
# Number of crystals in the assembly along X and Y axis
/detector/setCrystalNumberX 3
/detector/setCrystalNumberY 3

# Output of the locations, energies and particle types that escape the World boundary
# turn this setting off when performing a simulation with number of events > 100
/histoManager/writeWorldEscape true

# Incident particle type and energy
/gps/particle e-
/gps/ene/mono 5.2 GeV

# Number of events for the visualization run
/run/beamOn 15

# Uncomment lines below in order to draw the energy deposition projections for crystals and PMTs
/score/drawProjection crystalsMesh eneDepCrystal myColorMap
/score/drawProjection pmtsMesh eneDepPMT myColorMap

# Number of events in the main run (for energy resolution)
/run/beamOn 5000
```

Please note that there are two `/run/beamOn` comands. First command defines the number of the events for the visualization run. Usually 10-20 events are enough to obtain an energy deposition shower profile. Second `/run/beamOn` command generates output ROOT file with a larger number of events for obtaining the energy resolution. Please use about 10000 events for the energy resolution.

Once the above parameter values are set, hit `CTRL+X` to close nano editor and save the macro file. Program can be launched via following command:
```
glass
```

The Geant4 user interface window is presented to the user. Click on the "Open" bitton on the toolbar and locate the `./macros/farm.mac` macro file containing the above commands that control the simulation. 

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screenshot%20from%202020-12-21%2020-42-48.png?raw=true" alt="Specify the Geant4 Macro File" /><br/>
 <i>Click the `Open` button on the left of the toolbar and locate the `./mcaros/farm.mac` macro file.</i>
</p>

After the program run finishes the visualization window will be demonstarted to the user. Second run Visualization accumulates particle trajectories from a single event. However, the energy profiles correspond to the total number of the events specified in the macro file.

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screenshot%20from%202020-12-21%2020-45-41.png?raw=true" alt="Visualization Image of the Energy Deposition in the Crystals and PMT assembly" /><br/>
 <i>Visualization Image of the Energy Deposition in the Crystals and PMT assembly is automatically saved in `./output` folder</i>.
</p> 

### Analysis of the Output Files
Simulation results, namely the visualization image in EPS format as well as two output ROOT files are automatically saved under the `./output` folder.

The glass prototype Geant4 program code generated two ROOT files for every simulation. First ROOT file contains events from 15 injected particles by default and is used to visualize the shower profile. Additionally this file contains information about the particles escaped the simulation: location, energy and particle type.

Second ROOT file contains considerably more number of events (10k by default) and is used to calculate the energy resolution of the detector.

A special ROOT script is designed to analyze the output files. Command to launch the script is following:
```
root ./draw/energy-deposition/energyDeposition.cpp
```

Script can process both types of output files.

#### Analysis of a single file

Select a desired ROOT data file to be processed by the program. The ROOT file containing smaller number of the events (15 by default) stores information about the escape locations and energies of the particles escaping the world, as well as the particle types. This is important to ensure the energy balance in the simulation is correct.

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screenshot%20from%202020-12-22%2000-12-55.png?raw-true" alt="Visualization of the particles escaping the world." /><br/>
 <i>Visualization of the particles escaping the world.</i>
</p> 

Another ROOT file containing condiderably larger number of events (10000 by default) must be used for plotting the energy resolution of the assembly.

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screenshot%20from%202020-12-22%2000-43-16.png?raw=true" alt="Energy deposition in the crystals and Energy Resolution calculation" /><br/>
 <i>Energy deposition in the crystals and Energy Resolution calculation.</i>
</p> 

The graph with total energy deposition in the crystal assembly per event is fitted with the Crystal Ball function. Energy resolution of the detector is calculated as the ratio of the Crystal Ball functino mean to the sigma. 

#### Plotting Energy Resolution for Multiple Energies

Script `energyDeposition.cpp` will plot the energy resolution graph for a set of energies if multiple input ROOT files with large statistics are selected. Locate the `.root` output files that correspond to the simulations of the same detector geometry for different energies. It is possible to select multiple files in ROOT dialog box by checking the `Multiple files` checkbox on the top right of the dialog box and holding the `CTRL` or `CMD` key on the keyboard:

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screencast_01-12-2021_09 33 55 AM.gif?raw=true" alt="Selecting multiple files with ROOT File dialog." /><br/>
 <i>Selecting multiple files with ROOT File dialog.</i>
</p>

The ASCII data file with energy resolution values for every energy of the incident particle is created. Additionally the energy resolution graph is plotted.

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screenshot from 2021-01-12 21-38-45.png?raw=true" alt="Energy resolution plot for multiple incident particle energies" /><br/>
 <i>Energy resolution plot for multiple incident particle energies.</i>
</p>

All above graphs are automatically saved in the same folder where the input data files are located.

#### Plotting Energy Resolution for Multiple Geometries

A special Gnuplot script `./glass_prototype/draw/energy-resolution/energyResolution.gp` can plot series of the energy resolutions for various detector geometries. open the script with a text editor of your choice: 
```
nano ./draw/energy-resolution/energyResolution.gp
```

Update the data file names in the Gnuplot script to correspond to the energy resolution data points output by the `energyResolution.cpp` script. Then the energy resolution series can can be plotted with the following command:
```
gnuplot -p -c ./draw/energy-resolution/energyResolution.gp
```

A demonstration output of the Gnuplot script is presented on the picture below:

<p align="center">
 <img src="https://github.com/JeffersonLab/glass-prototype/blob/main/resources/Screenshot from 2021-01-18 15-32-12.png?raw=true" alt="Series of Energy resolutions for multiple detector geometries." /><br/>
 <i>Series of Energy resolutions for multiple detector geometries.</i>
</p>

### Copying the Results to Local Computer

In order to copy the results from the Computing Farm to the local computer the `scp` command can be used. On Windows one must install the PuTTY software to execute the below command. Native Terminal applications can be used on MacOS and Linux. Use following command to copy the Geant4 `output` folder to the local machine. Replace `<your-username>` with your JLab username:
```
scp -r <your-username>@login.jlab.org:/home/<your-username>/Downloads/glass-prototype-build/output ~/
```

In order to copy a single file, remove the `-r` flag from the above command.

## Notes from the Former Developers

For Hall C DVCS (DVCS_evt_gen/), see https://wiki.jlab.org/cuawiki/images/f/fa/User_Guide.pdf for a short description on how to run on JLab/ifarm
