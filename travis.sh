#!/usr/bin/env bash

echo "....fetching librdkafka dependency...."
mkdir tmp_build
cd tmp_build
wget https://github.com/edenhill/librdkafka/archive/0.9.1.tar.gz
tar -zxf 0.9.1.tar.gz
echo ".....done....."
cd librdkafka-0.9.1
echo "....compiling librdkafka...."
./configure && make
sudo make install
echo "....done...."
cd ../../
echo ".... ensure librdkafka is available....."
sudo ldconfig
rm -Rf tmp_build

echo "....fetching HDF5 dependency...."
sudo apt-get update -qq
apt-get -y install libhdf5-dev
apt-get -y install libprotobuf-dev
echo "....done...."
