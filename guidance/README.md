# Guidance Component

This is the folder containing the code for the guidance component.

## Prerequisites


### Protobuffers
Proto buffers need to be built and installed.
grpc needs to be built and installed.
If you clone the protobuf folder the installation can be done by executing the commands:

````
mkdir build
cd build
make
sudo make install
````

### MAVSDK

Install MAVSDK following the instructions for building from source:

````
https://mavsdk.mavlink.io/main/en/cpp/guide/build.html
````

A local install is preferred, but a global one is also possible.


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

Check if the examples work:

- Run an instance of px4 with a simulator
````
   make px4_sitl jmavsim
````
- From a different terminal run the offboard example and observe the output in the simulator
````
   ./offboard
````


## Build the code using cmake

Create a build folder and cd to it and run the following commands:
````
mkdir build
cd build
cmake ..
make
````

### Usage

To run the executable, use the provide script in the guidance folder:

````
    $ cd guidance
    $ ./start_guidance.sh
````
The guidance component will create a log file with the commands that are used to start it and with data that the component receives.