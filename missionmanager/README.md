# Missionmanager Component

This is the folder containing the code for the mission manager component.

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


## Build the code using cmake

Create a build folder and cd to it and run the following commands:
````
mkdir build
cd build
cmake ..
make
````

### Usage

To run the executable, use the provide script in the missionmanager folder:

````
    $ cd missionmanager
    $ ./start_missionmanager.sh
````
The mission manager component will create a log file with the commands that are used to start it and with data that the component receives.