#!/bin/bash

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
make doc

echo "Create package"
make package

cd ..

echo "Create RPM"
rm -rf package/*
mkdir -p package/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

cp build/isis_nexus_streamer-1.0.0.tar.gz package/SOURCES/
cp code/isis_nexus_streamer.spec package/SPECS/

rpmbuild \
    --define "_topdir $(pwd)/package" \
    --define "_version 1.0.0" \
    --define "_release $BUILD_NUMBER" \
    -bb package/SPECS/isis_nexus_streamer.spec
