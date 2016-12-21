#!/bin/bash

ls
pwd
ls

cd ..
mkdir -p build
cd build

echo "Run cmake..."
HDF5_ROOT=/opt/dm_group/hdf5 \
    cmake ../code \
    -DLibRDKafka_ROOT_DIR=/opt/dm_group/librdkafka

echo "Build project"
make

echo "Run tests"
./unitTests ../code/data/ --gtest_output=xml:test_results.xml

echo "Create docs"
make docs
