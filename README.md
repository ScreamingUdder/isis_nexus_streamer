[![Build Status](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer.svg?branch=master)](https://travis-ci.org/ScreamingUdder/isis_nexus_streamer) [![Coverage Status](https://coveralls.io/repos/github/ScreamingUdder/isis_nexus_streamer/badge.svg?branch=master)](https://coveralls.io/github/ScreamingUdder/isis_nexus_streamer?branch=master) [![License (2-Clause BSD)](https://img.shields.io/badge/license-BSD%202--Clause-blue.svg)](https://github.com/ScreamingUdder/isis_nexus_streamer/blob/master/LICENSE)

# ISIS NeXus Streamer
Stream event data from a NeXus file from RAL/ISIS using Apache Kafka. A producer and a consumer client are included. Each message sent over the wire consists of a single frame of event mode data.

Producer usage:
```
main_nexusPublisher -f <filepath>    NeXus filename including full path
               [-b <host>]    Hostname of a broker in the Kafka cluster
               [-t <topic_name>]    Name of the topic to publish to
               [-m <messages_per_frame>]    Number of messages per frame
               [-q]    Quiet mode, make publisher less chatty
               [-u]    Random mode, serve messages within each frame in a random order
```

Consumer usage:
```
main_nexusSubscriber
               [-b <host>]    NeXus filename including full path
               [-t <topic_name>]    Name of the topic to subscribe to
               [-f <filepath>]    Write the received data to a NeXus file with this full path
               [-q]    Quiet mode, make the client less chatty
```

## Dependencies
Currently requires having `librdkafka` and the HDF5 C++ library installed. If `tcmalloc` is available then it will be used, but it is not a requirement.

`Google Test` and `Google Mock` are used for unit testing but are not required to be installed; CMake will download and build them at configure-time.

Due to large file sizes altering the test data requires `git lfs` to be installed:
https://git-lfs.github.com/

## Unit tests
The unit test executable `unitTests` needs to be passed the path of the test data **directory** as an argument.
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
- numpy (`pip install numpy`)
- matplotlib (`pip install matplotlib`)
- kazoo (`pip install kazoo`)
