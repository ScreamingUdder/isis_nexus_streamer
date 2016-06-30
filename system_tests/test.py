import os
import test_utils
import sys
from git import Repo
import vagrant

# Redirect stdout to a system test output file
# sys.stdout = open('system_test_output.txt', 'w')

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

#    [os.path.join(consumer_path, "main_nexusSubscriber"),
#     "-b", "localhost"])


# Build directory as an input argument
build_dir = os.path.join(os.getcwd())
#build_dir = "/home/jonmd/.CLion2016.2/system/cmake/generated/isis_nexus_streamer_git-45f5f72f/45f5f72f/Release/"

# topic should have no messages in the log before test runs, how can this be enforced?
# or be able to pass starting offset to consumer

# Start the stopwatch

# Launch the consumer
consumer_path = os.path.join(build_dir, "nexus_consumer")
# TODO pass args "-b localhost:2181 -t system_test"
#consumer_process = test_utils.Subprocess(
#    [os.path.join(consumer_path, "main_nexusSubscriber"),
#     "-b", "localhost"])

# Run the producer
producer_path = os.path.join(build_dir, "nexus_producer")
# TODO pass args "-b localhost:9092 -t system_test"
#producer_process = test_utils.Subprocess(
#    [os.path.join(producer_path, "main_nexusPublisher"),
#     "-f", "/home/jonmd/git/isis_nexus_streamer.git/data/SANS_test.nxs",
#     "-b", "localhost"])

# Wait for the consumer subprocess to complete
#consumer_process.wait()
#print consumer_process.output

# Stop the stopwatch

# Check the output from the consumer (consumer_process.output)
# Did all messages (frames) arrive?

# Make sure the producer also finished
#producer_process.wait()
#print producer_process.output

# Shut down the virtual cluster
print "Shutting down virtual cluster..."
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.halt()
print "...virtual cluster is down."
