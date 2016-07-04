import os
import test_utils
import sys
from git import Repo
import vagrant

if len(sys.argv) is not 2:
    print "System test script requires the full path of the test data directory containing SANS_test.nxs"
    exit()

data_path = str(sys.argv[1])
print "Path of data directory was given as " + data_path

# Redirect stdout to a system test output file
sys.stdout = open('system_test_output.txt', 'w')

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

# Mark the test topic for deletion
with test_utils.cd(repo_dir):
    delete_topic_process = test_utils.Subprocess([
        "vagrant", "ssh",
        "kafka-node-1",
        "--command",
        "\"/etc/kafka_2.11-0.9.0.1/bin/kafka-topics.sh --zookeeper zk-node-1:2181 --delete --topic system_test\""
    ])
    delete_topic_process.wait()
    print "marked for deletion successfully"

# Start the stopwatch

# Launch the consumer
consumer_process = test_utils.Subprocess(
    [os.path.join(build_dir, "nexus_consumer", "main_nexusSubscriber"),
     "-b", "localhost",
     "-t", "system_test"])

# Run the producer
producer_process = test_utils.Subprocess(
    [os.path.join(build_dir, "nexus_producer", "main_nexusPublisher"),
     "-f", os.path.join(data_path, "SANS_test.nxs"),
     "-b", "localhost",
     "-t", "system_test"])

# Wait for the consumer subprocess to complete
consumer_process.wait()
print consumer_process.output

# Stop the stopwatch

# Check the output from the consumer (consumer_process.output)
# Did all messages (frames) arrive?

# Make sure the producer also finished
producer_process.wait()
print producer_process.output

# Mark the test topic for deletion
with test_utils.cd(repo_dir):
    delete_topic_process = test_utils.Subprocess([
        "vagrant", "ssh",
        "kafka-node-1",
        "--command",
        "\"/etc/kafka_2.11-0.9.0.1/bin/kafka-topics.sh --zookeeper zk-node-1:2181 --delete --topic system_test\""
    ])
    delete_topic_process.wait()

# Shut down the virtual cluster
print "Shutting down virtual cluster..."
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.halt()
print "...virtual cluster is down."

print "End of system tests"
