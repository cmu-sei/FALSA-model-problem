# Payload Component

This is the folder containing the code for the payload component.

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

To run executable:

````
    $ cd payload
    $ ./start_payload.sh
````