[![Build Status](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer.svg?branch=master)](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer)

# ISIS NeXus Streamer
Stream event data from a NeXus file from RAL/ISIS.
Unit tests use Google Test.

## Dependencies
Currently requires having `librdkafka` and the HDF5 C++ library installed.
`gtest` is not required to be installed, CMake will download and build it at configure-time.

## Running the unit tests
The unit test executable `unitTests` needs to be passed the path of the test data directory as an argument.
Alternatively, run all units tests using ctest with
```
ctest -VV
```
from the build directory.
