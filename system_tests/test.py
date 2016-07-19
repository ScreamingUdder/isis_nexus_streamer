from __future__ import print_function
import os
import test_utils
import sys
from git import Repo
import vagrant
import time

if len(sys.argv) is not 2:
    print("System test script requires the full path of the test data directory containing SANS_test.nxs")
    exit()

# Redirect stdout to a system test output file
#sys.stdout = open('system_test_output.txt', 'w')

data_path = str(sys.argv[1])
print("Path of data directory was given as " + data_path)

topic_name = "topic_system_test"
broker = "localhost"

# Clone ansible-kafka-centos git repo
# if already exists in build director make sure up-to-date by pulling master
repo_dir = "ansible-kafka-centos"
try:
    Repo.clone_from("https://github.com/ScreamingUdder/ansible-kafka-centos.git", repo_dir)
except:
    virtual_cluster_repo = Repo(repo_dir)
    origin = virtual_cluster_repo.remotes['origin']
    origin.pull()

# Start up the virtual cluster
print("Starting up the virtual cluster...")
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.up()
print("...virtual cluster is up.")

# Build directory as an input argument
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
     "-f", os.path.join(data_path, "SANS_test.nxs"),
     "-b", broker,
     "-t", topic_name,
     "-q"])
print(" done.")

# Collect metrics from broker
print("Collecting metrics from broker...")
with test_utils.cd("system_tests"):
    jmx = test_utils.JmxMetrics("localhost:9990")
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
print("Total time taken to send and received all event data is " + str(total_time) + " seconds")

# Make sure the producer also finished
print("Checking producer has finished...")
producer_process.wait()
print("...producer process completed.")

# Check the output from the consumer (consumer_process.output)
# Did all messages (frames) arrive?
all_messages_received = True
# Remove any lines before one starting with "message: "
try:
    producer_frames = producer_process.output.split("Frames sent: ", 1)[1].split('\n', 1)[0]
    producer_bytes = producer_process.output.split("Bytes sent: ", 1)[1].split('\n', 1)[0]
    consumer_frames = consumer_process.output.split("Frames received: ", 1)[1].split('\n', 1)[0]
    consumer_bytes = consumer_process.output.split("Bytes received: ", 1)[1].split('\n', 1)[0]

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

# Shut down the virtual cluster
print("Shutting down virtual cluster...")
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.halt()
print("...virtual cluster is down.")

print("End of system tests")
