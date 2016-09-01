from __future__ import print_function
import os
import test_utils
import sys
from git import Repo
import time
import argparse
import matplotlib.pyplot as pl

"""
Launch with:
python test.py /path/to/test/data [-p]
The "-p" flag executes on the physical cluster rather than launching virtual machines
"""


def check_all_received(producer_process_output, consumer_process_output):
    """
    Check the output from the consumer (consumer_process.output)
    Did all messages and frames arrive?
    """
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


def get_single_metric(jmxhosts):
    """
    Get a single value using JmxTerm
    """
    with test_utils.cd("system_tests"):
        # Wait 90 seconds, then get the average rates from last minute
        time.sleep(90)
        for host in jmxhosts:
            jmx = test_utils.JmxMetrics(host)
            print(jmx.get_metric("BrokerTopicMetrics", "BytesInPerSec", "OneMinuteRate"))


def finish_collect_and_plot_metrics(jmxhosts, jmxtool_cpu, jmxtool_broker, topic_name):
    """
    Finish collecting metrics with JmxTool and plot them
    """
    for host in jmxhosts:
        test_utils.plot_metrics(jmxtool_cpu[host].get_output(), ylabel="CPU use [%]", title=host, yscale=100)
        try:
            test_utils.plot_metrics(jmxtool_broker[host].get_output(),
                                    ylabel="broker in and out, 1 minute average [Mbps]",
                                    title=host,
                                    yscale=8e-6)
        except IndexError:
            print(
                "Warning: plotting metric for " + host +
                ", probably the topic (" + topic_name + ") does not exist on this broker")


def parse_arguments():
    parser = argparse.ArgumentParser(description='Run system tests.')
    parser.add_argument('data_path', type=str,
                        help='the full path of the test data directory')
    parser.add_argument('-f', '--datafile', default='SANS_test_uncompressed.hdf5', type=str,
                        help='name of the NeXus file to stream')
    parser.add_argument('-o', '--output_file', default='systemtest_received_file.hdf5', type=str,
                        help='name of the file to output from the consumer')
    parser.add_argument('-p', action='store_true',
                        help='stream data across physical cluster instead of virtual cluster')
    parser.add_argument('-t', '--topic_name', default='topic_system_test', type=str,
                        help='name of the topic to use, default is "topic_system_test"')
    parser.add_argument('-g', '--producer_only', action='store_true',
                        help='use g flag to launch the producer but not consumer')
    parser.add_argument('-r', '--random', action='store_true',
                        help='publish messages within each frame in a random order, for testing message reordering')
    parser.add_argument('-z', '--zookeeper', default='localhost', type=str,
                        help='hostname of the Zookeeper')
    return parser.parse_args()


def create_jmxhosts_list(args):
    return [broker + ':' + str(jmxport) for (broker, jmxport) in zip(args.info.brokers(), args.info.jmxports())]


def pull_virtual_cluster_repo(args):
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
    return repo_dir


def launch_consumer(args):
    if not args.producer_only:
        print("Launching consumer...", end="")
        consumer_process = test_utils.KafkaSubprocess(
            [os.path.join(args.build_dir, "nexus_consumer", "main_nexusSubscriber"),
             "-b", args.broker,
             "-t", args.topic_name,
             "-f", os.path.join(args.data_path, args.output_file),
             "-q"])
        print(" done.")
        return consumer_process
    return None


def launch_producer(args):
    print("Launching producer...", end="")
    if args.random:
        producer_process = test_utils.KafkaSubprocess(
            [os.path.join(args.build_dir, "nexus_producer", "main_nexusPublisher"),
             "-f", os.path.join(args.data_path, args.datafile),
             "-b", args.broker,
             "-t", args.topic_name,
             "-m", "200",
             "-r",
             "-q"])
    else:
        producer_process = test_utils.KafkaSubprocess(
            [os.path.join(args.build_dir, "nexus_producer", "main_nexusPublisher"),
             "-f", os.path.join(args.data_path, args.datafile),
             "-b", args.broker,
             "-t", args.topic_name,
             "-m", "200",
             "-q"])
    print(" done.")
    return producer_process


def main():
    args = parse_arguments()
    args.info = test_utils.KafkaInfo(args.zookeeper)
    args.broker = args.info.brokers()[0]

    # Redirect stdout to a system test output file
    # sys.stdout = open('system_test_output.txt', 'w')

    repo_dir = pull_virtual_cluster_repo(args)

    # Start up the virtual cluster
    with test_utils.Cluster(repo_dir):
        args.build_dir = os.path.dirname(os.path.realpath(sys.argv[0])).rsplit('/', 1)[0]
        print(args.build_dir)

        jmxhosts = create_jmxhosts_list(args)

        print("Start collecting metrics from broker...")
        jmxtool_broker = {}
        jmxtool_cpu = {}
        for host in jmxhosts:
            jmxtool_broker[host] = test_utils.JmxTool(args.build_dir, host, topic=args.topic_name)
            jmxtool_cpu[host] = test_utils.JmxTool(args.build_dir, host, metrics="cpu")

        # Start the stopwatch and launch clients
        t0 = time.time()
        consumer_process = launch_consumer(args)
        time.sleep(2)
        producer_process = launch_producer(args)

        # demonstrate collecting a single metric value during the tests
        # get_single_metric(jmxhosts)

        # Wait for the consumer subprocess to complete
        if not args.producer_only:
            print("Waiting for consumer to finish...")
            consumer_process.wait()
            print("...consumer process completed.")

        # Make sure the producer also finished
        print("Checking producer has finished...")
        producer_process.wait()
        print("...producer process completed.")

        # Stop the stopwatch
        t1 = time.time()
        total_time = t1 - t0
        print("Total time taken to send and receive all event data is " + str(total_time) + " seconds")

        finish_collect_and_plot_metrics(jmxhosts, jmxtool_cpu, jmxtool_broker, args.topic_name)

        if not args.producer_only:
            # check_all_received(producer_process.output, consumer_process.output)
            test_utils.nexus_files_equal(os.path.join(args.data_path, args.datafile),
                                         os.path.join(args.data_path, args.output_file))

        # Shut down the virtual cluster
        print("Shutting down virtual cluster...")

    print("...virtual cluster is down.")

    print("End of system tests")


if __name__ == "__main__":
    main()
    pl.show()
