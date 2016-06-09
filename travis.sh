#!/usr/bin/env bash

echo "....fetching librdkafka dependency...."
mkdir tmp_build
cd tmp_build
git clone https://github.com/edenhill/librdkafka.git
echo ".....done....."
cd librdkafka
echo "....compiling librdkafka...."
./configure && make
sudo make install
echo "....done...."
cd ../../
echo ".... ensure librdkafka is available....."
sudo ldconfig
rm -Rf tmp_build
