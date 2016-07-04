import os
import test_utils
import sys
from git import Repo
import vagrant
import time

if len(sys.argv) is not 2:
    print "System test script requires the full path of the test data directory containing SANS_test.nxs"
    exit()

# Redirect stdout to a system test output file
sys.stdout = open('system_test_output.txt', 'w')

data_path = str(sys.argv[1])
print "Path of data directory was given as " + data_path

topic_name = "system_test"

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
t0 = time.time()

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
# Stop the stopwatch
t1 = time.time()
total_time = t1 - t0
print "Total time taken to send and received all event data is " + str(total_time) + " seconds"

# Make sure the producer also finished
producer_process.wait()

# Check the output from the consumer (consumer_process.output)
# Did all messages (frames) arrive?
all_messages_received = True
# Remove any lines before one starting with "message: "
producer_output = "message: " + producer_process.output.split("message: ", 1)[1]
consumer_output = "message: " + consumer_process.output.split("message: ", 1)[1]
for send_line, recv_line in zip(producer_output.splitlines(), consumer_output.splitlines()):
    if send_line.split("message: ", 1)[1] != recv_line.split("message: ", 1)[1]:
        print "Received message (" + send_line.split("message: ", 1)[1] + ") does not match sent message (" + \
              recv_line.split("message: ", 1)[1] + ")"
        all_messages_received = False
if all_messages_received:
    print "Received and sent messages match in size and frame number."

# Shut down the virtual cluster
print "Shutting down virtual cluster..."
with test_utils.cd(repo_dir):
    v = vagrant.Vagrant(quiet_stdout=False)
    v.halt()
print "...virtual cluster is down."

print "End of system tests"
