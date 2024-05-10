# LTL Monitor Runtime
This code loads a monitor generated with `ltlmon` and accepts notifications of
actions or events to check whether the LTL property checked by the monitor is
violated.

A system using this monitor only needs the `LTLMonitor` class in the files
`ltlmonrt.hpp` and `ltlmonrt.cpp`.

Use of this class is shown in `main.cpp` that uses the `LTLMonitor` class to
test it. Basically:
1. create `LTLMonitor` instance
2. invoke the method `initialize()` to load the `.mon` file for the monitor
3. for each action/event in the system, invoke the `step()` method
4. check the return value of `step()` a `false` means that the property was violated 


## Build

```
mkdir build
cd build
cmake ..
cd ..
cmake --build build/
```

## Run

```
build/ltlmonrt  
```
