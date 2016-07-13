[![Build Status](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer.svg?branch=master)](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer) [![Coverage Status](https://coveralls.io/repos/github/ScreamingUdder/isis_nexus_streamer/badge.svg?branch=master)](https://coveralls.io/github/ScreamingUdder/isis_nexus_streamer?branch=master) [![License (2-Clause BSD)](https://img.shields.io/badge/license-BSD%202--Clause-blue.svg)](https://github.com/ScreamingUdder/isis_nexus_streamer/blob/master/LICENSE)

# ISIS NeXus Streamer
Stream event data from a NeXus file from RAL/ISIS using Apache Kafka. A producer and a consumer client are included. Each message sent over the wire consists of a single frame of event mode data.

## Dependencies
Currently requires having `librdkafka` and the HDF5 C++ library installed.

`Google Test` and `Google Mock` are used for unit testing but are not required to be installed; CMake will download and build them at configure-time.

Due to large file sizes altering the test data requires `git lfs` to be installed:
https://git-lfs.github.com/

## Unit tests
The unit test executable `unitTests` needs to be passed the path of the test data directory as an argument.
Alternatively, run all units tests using ctest with
```
ctest -VV
```
from the build directory.

## System tests
The CMake target for the system tests is `system_test`. Running the tests has the following dependencies:
- Vagrant (`apt-get install vagrant`)
- Ansible (`apt-get install ansible`)
- Python 2.7
- JRE (`apt-get install default-jre`)
- gitpython (`pip install gitpython`)
- python-vagrant (`pip install python-vagrant`)
