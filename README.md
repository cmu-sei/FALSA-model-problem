# Model Problem Design and Implementation Document

> **Note:** This guide contains the details of how the model problem is built and run, but for an easier environment, look at this other [README-devcontainer.md](README-devcontainer.md).

The model problem consists of several components located in their own subfolders.
Each subfolder has its own CMakeLists.txt file.
The general procedure for building a component is the following:

````
mkdir build
cd build
cmake ..
make
````

If a source file is modified then simply typing 'make' in the build folder can rebuild the project.
If the CMakeLists.txt is not modified then 'cmake ..' does not need to be run again if it was already run.


## Prerequisites


### CMake

The project uses cmake and as some of the components such as gRPC and MAVSDK require a newer version of cmake it may be necessary to upgrade your existing cmake installation. The instructions for gRPC and MAVSDK discuss these requirements.CMake can be installed from source or through other easier methods.


### Protobuffers
Proto buffers need to be built and installed.
If you clone the protobuf folder the installation can be done by executing the commands:

````
mkdir build
cd build
make
sudo make install
````

### gRPC
grpc needs to be built and installed.
Follow the instruction for installing it on your OS.

````
https://grpc.io/docs/languages/cpp/quickstart/
````

### Poco

Poco C++ is a general purpose library that provides different facilities for building applications in C++.

Installing it on your OS can happen by following the following instructions:

https://pocoproject.org/download.html


### MAVSDK

Install MAVSDK following the instructions for building from source:

````
https://mavsdk.mavlink.io/main/en/cpp/guide/build.html
````

A local install is possible, but a global one is also possible and it is even better.
This will get the latest code from the master branch of MAVSDK.

Patch the offboard example (offboard.cpp)

Add the following include clause at the begining of the file:
````
#include <mavsdk/plugins/param/param.h>
````

Add the following change in main():

````
// Set this parameter to avoid failure mode
auto param = Param(std::shared_ptr<System>(system.value()));
param.set_param_int("COM_RCL_EXCEPT", 4);

````

Check if the examples build properly.
If not set the following environment variable first and then run the build process for the examples:

````
export MAVLink_DIR="~/MAVSDK/build/default/third_party/install/lib/cmake/MAVLink/"
````

Check if the examples work. For this one needs to install PX4 (See the next section):

- Run an instance of PX4 with a simulator
````
   make px4_sitl jmavsim
````
- From a different terminal run the offboard example and observe the output in the simulator
````
   ./offboard
````


### PX4

Install PX4 in your OS following the instructions on this page:

````
https://docs.px4.io/main/en/dev_setup/building_px4.html
````

Starting PX4 with the default JMavsim simulator can be done with the following command:

````
make px4_sitl jmavsim
````


## Folder Structure

### Common Folders

The following folders exist in the modelproblem folder:

````
.
├── assurancebrkr
├── configs
├── gcs
├── guidance
├── interfaces
├── missionmanager
├── payload
├── protos
└── utils
````

They have the following roles:

  + assurancebrkr - Assurance Broker Component  
  + configs - Contains common configuration files
  + interfaces - Contains common header files
  + protos - Contains .proto files, used by gRPC
  + gcs - Ground Control Station Component
  + missionmanager - Mission Manager Component
  + guidance - Guidance Component
  + payload - Payload Controller Component
  + utils - Contains a port parsing utility class



## Build the code

There is a high level CMakeLists.txt file that builds all subfolders using their CMakeLists.txt files.
The code can be build as we build any CMake project:
````
mkdir build
cd build
cmake ..
make
````
Alternatively each subfolder can be used to build each individual component using the following procedure:
````
cd subfolder
mkdir build
cd build
cmake ..
make
````

### Starting individual components

Run the script that starts each process created through the build process from the respective folder.
Each component has a shell script for starting it.
For example to start the guidance component run its shell script.

````
    $ cd guidance
    $ ./start_guidance.sh
````

All components can be started in individual terminal windows to enable output and debugging.
The components that exist are:

  + assurancebrkr - Assurance Broker Component
  + gcs - Ground Control Station Component
  + missionmanager - Mission Manager Component
  + guidance - Guidance Component
  + payload - Payload Controller Component


In addition we have the PX4 run in a separate terminal with JMavSim.

### How to launch the model problem software


  + Start 6 terminal windows
  + Start PX4 in one of the terminal windows with the command: make px4_sitl jmavsim
  + After PX4 has completely started and is ready to be used, in the second terminal window navigate to guidance folder and execute: ./start_guidance.sh
  + In the third terminal window navigate to the missionmanager window and execute start_missionmanager.sh script   
  + In the forth terminal window navigate to the payload folder and start the app by executing start_payload.sh
  + In the fifth terminal window navigate to gcs folder and execute the start_gcs.sh script
  + In the sixth terminal window navigate to assurancebrkr folder and execute the start_assurancebrkr.sh script
  + Execute first ./start_gcs.sh setmissionparams. The following parameters work well for an example or demonstration:
      + Altitude: 1.7
      + Latitude: 47.4
      + Longitude: 8.55
      + Start Time: 0
      + End Time: 10
  + Execute second ./start_gcs.sh takeoff
  + The clearmissionparams and abort can also be issued if needed before or during a mission respectively


### Debugging

Debugging can happen through different IDEs.
One popular example can be the Eclipse IDE for C++.
Another one is Visual Studio Code.
Yet another possibility is to use a commercial tool such as IntelliJ IDE for C++.
If the code is built with debugging symbols (Add the -g to the compiler parameters in your CMakefile.txt) one can also use gdb as a bare bones approach to debugging.


### Logging
Logging is supported by the Poco C++ library.
The format of logging for individual components needs to be finalized based on the needs of the scientific machine learning algorithms.


