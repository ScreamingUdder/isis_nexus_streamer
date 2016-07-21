from __future__ import print_function
import os
import test_utils
import sys
from git import Repo
import time
import argparse

"""
Launch with:
python test.py /path/to/test/data [-p]
The "-p" flag executes on the physical cluster rather than launching virtual machines
"""


def check_all_received(producer_process_output, consumer_process_output):
    # Check the output from the consumer (consumer_process.output)
    # Did all messages and frames arrive?
    try:
        producer_frames = producer_process_output.split("Frames sent: ", 1)[1].split('\n', 1)[0]
        producer_bytes = producer_process_output.split("Bytes sent: ", 1)[1].split('\n', 1)[0]
        consumer_frames = consumer_process_output.split("Frames received: ", 1)[1].split('\n', 1)[0]
        consumer_bytes = consumer_process_output.split("Bytes received: ", 1)[1].split('\n', 1)[0]

        if producer_frames != consumer_frames:
            print("FAIL " + producer_frames + " frames sent but " + consumer_frames + " frames received")
            raise IndexError("")
        elif producer_bytes != consumer_bytes:
            print("FAIL " + producer_bytes + " bytes sent but " + consumer_bytes + " bytes received")
            raise IndexError("")
        else:
            print("PASS " + producer_frames + " frames sent and " + consumer_frames + " frames received.")
            print(producer_bytes + " bytes sent and " + consumer_bytes + " bytes received.")
    except IndexError:
        print("Unexpected output from producer or consumer, try running unit tests")
        sys.exit()


def main():

    parser = argparse.ArgumentParser(description='Run system tests.')
    parser.add_argument('data_path', type=str,
                        help='the full path of the test data directory')
    parser.add_argument('-p', action='store_true',
                        help='use p flag to stream data across physical cluster instead of virtual cluster')
    parser.add_argument('-j', '--jmxport', type=str, default="9990",
                        help='specify the port on the broker for jmx')
    args = parser.parse_args()

    # Redirect stdout to a system test output file
    #sys.stdout = open('system_test_output.txt', 'w')

    topic_name = "topic_system_test"
    broker = "localhost"
    #datafile = "SANS_test.nxs"
    datafile = "WISH00034509_uncompressed.hdf5"

    repo_dir = "ansible-kafka-centos"
    # Clone git repository
    # if already exists in build director make sure up-to-date by pulling master
    if not args.p:
        try:
            Repo.clone_from("https://github.com/ScreamingUdder/ansible-kafka-centos.git", repo_dir)
        except:
            virtual_cluster_repo = Repo(repo_dir)
            origin = virtual_cluster_repo.remotes['origin']
            origin.pull()
        print("Starting up the virtual cluster...")
    else:
        print("Using real cluster")
        repo_dir = ""
        broker = "sakura"

    # Start up the virtual cluster
    with test_utils.Cluster(repo_dir):
        build_dir = os.path.join(os.getcwd())

        # Start the stopwatch
        t0 = time.time()

        # Launch the consumer
        print("Launching consumer...", end="")
        consumer_process = test_utils.Subprocess(
            [os.path.join(build_dir, "nexus_consumer", "main_nexusSubscriber"),
             "-b", broker,
             "-t", topic_name,
             "-q"])
        print(" done.")

        time.sleep(2)

        # Run the producer
        print("Launching producer...", end="")
        producer_process = test_utils.Subprocess(
            [os.path.join(build_dir, "nexus_producer", "main_nexusPublisher"),
             "-f", os.path.join(args.data_path, datafile),
             "-b", broker,
             "-t", topic_name,
             "-q"])
        print(" done.")

        # Collect metrics from broker
        print("Collecting metrics from broker...")
        with test_utils.cd("system_tests"):
            jmx = test_utils.JmxMetrics(broker + ":" + args.jmxport)
            # Wait 60 seconds, then get the average rates from last minute
            time.sleep(60)
            print(jmx.get_metric("BrokerTopicMetrics", "BytesInPerSec", "OneMinuteRate"))
        print("...done collecting metrics.")

        # Wait for the consumer subprocess to complete
        print("Waiting for consumer to finish...")
        consumer_process.wait()
        print("...consumer process completed.")
        # Stop the stopwatch
        t1 = time.time()
        total_time = t1 - t0
        print("Total time taken to send and receive all event data is " + str(total_time) + " seconds")

        # Make sure the producer also finished
        print("Checking producer has finished...")
        producer_process.wait()
        print("...producer process completed.")

        check_all_received(producer_process.output, consumer_process.output)

        # Shut down the virtual cluster
        print("Shutting down virtual cluster...")

    print("...virtual cluster is down.")

    print("End of system tests")


if __name__ == "__main__":
    main()
