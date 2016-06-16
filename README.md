[![Build Status](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer.svg?branch=master)](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer) [![Coverage Status](https://coveralls.io/repos/github/ScreamingUdder/isis_nexus_streamer/badge.svg?branch=master)](https://coveralls.io/github/ScreamingUdder/isis_nexus_streamer?branch=master)

# ISIS NeXus Streamer
Stream event data from a NeXus file from RAL/ISIS using Apache Kafka.

## Dependencies
Currently requires having `librdkafka` and the HDF5 C++ library installed.

`Google Test` and `Google Mock` are used for unit testing but are not required to be installed; CMake will download and build them at configure-time.

The repository contains a NeXus file for testing and therefore requires `git lfs` to be installed:
https://git-lfs.github.com/

## Running the unit tests
The unit test executable `unitTests` needs to be passed the path of the test data directory as an argument.
Alternatively, run all units tests using ctest with
```
ctest -VV
```
from the build directory.
