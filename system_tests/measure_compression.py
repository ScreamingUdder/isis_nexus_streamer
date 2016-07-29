from __future__ import print_function
import test_utils
import time
import os
import pexpect
import csv
import numpy as np

"""
Crude measurement of the compression ratio achieved.
Carried out by monitoring network bytesin and bytesout vs time and integrating the curve.

Requires ifstat to be installed
"""


class Ifstat:

    def __init__(self, network_interface):
        self.ifstat = pexpect.spawn("ifstat -n -i " + network_interface)

    def get_output(self):
        # Get all output
        result = ""
        try:
            for n in range(1, 10):
                result = result + self.ifstat.read_nonblocking(size=16777216, timeout=2)
        except:
            print("Error reading from ifstat, buffer too small?")
        return result

    def __del__(self):
        self.ifstat.close(force=True)


def stream_data(build_dir, broker, topic_name, network_interface, data_path, datafile, compression):
    # Launch the consumer
    print("Launching consumer...", end="")
    consumer_process = test_utils.Subprocess(
        [os.path.join(build_dir, "nexus_consumer", "main_nexusSubscriber"),
         "-b", broker,
         "-t", topic_name,
         "-q"])
    print(" done.")

    time.sleep(2)

    # Run ifstat to monitor network use
    ifstat = Ifstat(network_interface)

    # Run the producer
    print("Launching producer...", end="")
    if compression:
        producer_process = test_utils.Subprocess(
            [os.path.join(build_dir, "nexus_producer", "main_nexusPublisher"),
             "-f", os.path.join(data_path, datafile),
             "-b", broker,
             "-t", topic_name,
             "-m", "100",
             "-c", compression,
             "-q"])
    else:
        producer_process = test_utils.Subprocess(
            [os.path.join(build_dir, "nexus_producer", "main_nexusPublisher"),
             "-f", os.path.join(data_path, datafile),
             "-b", broker,
             "-t", topic_name,
             "-m", "100",
             "-q"])
    print(" done.")

    print("Waiting for consumer to finish...")
    consumer_process.wait()
    print("...consumer process completed.")

    ifstat_output = ifstat.get_output()

    print("Waiting for producer to finish...")
    producer_process.wait()
    print("...producer process completed.")

    output_strings = ifstat_output.splitlines()[2:]
    data = []
    for line in output_strings:
        data.append([float(i) for i in line.split()])
    data = np.array(data)
    print(str(sum(data[:, 0])) + ' KB sent')
    print(str(sum(data[:, 1])) + ' KB received')

    return sum(data[:, 0])


def main():

    build_dir = "/home/jonmd/build_dir/Release/"
    data_path = "/home/jonmd/git/isis_nexus_streamer.git/data/"
    datafile = "WISH00034509_uncompressed.hdf5"
    broker = "tenten"
    topic_name = "compression_ratio_measure"
    network_interface = "enp0s31f6"

    size_no_compression = stream_data(build_dir, broker, topic_name, network_interface, data_path, datafile, None)
    size_with_compression = stream_data(build_dir, broker, topic_name, network_interface, data_path, datafile, "snappy")
    print("Compression ratio of " + str(size_no_compression/size_with_compression))


if __name__ == "__main__":
    main()
