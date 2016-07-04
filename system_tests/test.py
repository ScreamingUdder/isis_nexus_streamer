import os
import test_utils
import sys
from git import Repo
import vagrant
import datetime

if len(sys.argv) is not 2:
    print "System test script requires the full path of the test data directory containing SANS_test.nxs"
    exit()

# Redirect stdout to a system test output file
sys.stdout = open('system_test_output.txt', 'w')

data_path = str(sys.argv[1])
print "Path of data directory was given as " + data_path

# Create a unique topic name, ensures clean topic for test without problematic topic deletion
topic_base_name = "system_test"
topic_name = topic_base_name + "_" + datetime.datetime.now().strftime("%y%m%d_%H%M%S")

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
print "Starting up the virtual cluster..."
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.up()
print "...virtual cluster is up."

# Build directory as an input argument
build_dir = os.path.join(os.getcwd())

# Start the stopwatch

# Launch the consumer
consumer_process = test_utils.Subprocess(
    [os.path.join(build_dir, "nexus_consumer", "main_nexusSubscriber"),
     "-b", "localhost",
     "-t", topic_name])

# Run the producer
producer_process = test_utils.Subprocess(
    [os.path.join(build_dir, "nexus_producer", "main_nexusPublisher"),
     "-f", os.path.join(data_path, "SANS_test.nxs"),
     "-b", "localhost",
     "-t", topic_name])

# Wait for the consumer subprocess to complete
consumer_process.wait()
print consumer_process.output

# Stop the stopwatch

# Check the output from the consumer (consumer_process.output)
# Did all messages (frames) arrive?

# Make sure the producer also finished
producer_process.wait()
print producer_process.output

# Shut down the virtual cluster
print "Shutting down virtual cluster..."
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.halt()
print "...virtual cluster is down."

print "End of system tests"
