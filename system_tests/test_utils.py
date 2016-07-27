import os
import subprocess
import pexpect
import vagrant
import matplotlib.pyplot as pl
import numpy as np
import csv


def plot_metrics(results, ylabel="", title="", yscale=1):
    pl.figure()
    reader = csv.reader(results.splitlines())
    data = []
    for row in reader:
        data.append(row)
    data = np.array(data)
    pl.plot((data[1:, 0].astype(float) - data[1, 0].astype(float)) * 1e-3, data[1:, 1:].astype(float) * yscale)
    pl.xlabel("time [s]")
    pl.ylabel(ylabel)
    pl.title(title)


class Subprocess:
    def __init__(self, command, working_dir=None, env=None):
        """Changes into a specified directory, if provided, and executes a command.
        Restores the old directory afterwards.
        Args:
          command:        The command to run, in the form of sys.argv.
          working_dir:    The directory to change into.
          capture_stderr: Determines whether to capture stderr in the output member
                          or to discard it.
          env:            Dictionary with environment to pass to the subprocess.
        Returns:
          An object that represents outcome of the executed process. It has the
          following attributes:
            terminated_by_signal   True if the child process has been terminated
                                   by a signal.
            signal                 Signal that terminated the child process.
            exited                 True if the child process exited normally.
            exit_code              The code with which the child process exited.
            output                 Child process's stdout and stderr output
                                   combined in a string.
        """

        # The subprocess module is the preferable way of running programs
        # since it is available and behaves consistently on all platforms,
        # including Windows. Note it is only available starting in python 2.4.

        self.p = subprocess.Popen(command,
                                  stderr=subprocess.STDOUT, stdout=subprocess.PIPE,
                                  cwd=working_dir, universal_newlines=True, env=env)

    def wait(self):
        """
        Blocks until the process has exited
        """
        # communicate returns a tuple with the file object for the child's
        # output.
        self.output = self.p.communicate()[0]
        _return_code = self.p.returncode

        if _return_code < 0:
            self.terminated_by_signal = True
            self.exited = False
            self.signal = -_return_code
        else:
            self.terminated_by_signal = False
            self.exited = True


class cd:
    """
    Context manager for changing the current working directory
    """

    def __init__(self, new_path):
        self.new_path = os.path.expanduser(new_path)

    def __enter__(self):
        self.saved_path = os.getcwd()
        os.chdir(self.new_path)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.saved_path)


class Cluster:
    """
    Manage the virtual cluster
    """

    def __init__(self, repo_dir):
        self.repo_dir = repo_dir

    def __enter__(self):
        if self.repo_dir:
            with cd(self.repo_dir):
                v = vagrant.Vagrant(quiet_stdout=False)
                v.up()
                print("...virtual cluster is up.")

    def __exit__(self, etype, value, traceback):
        if self.repo_dir:
            with cd(self.repo_dir):
                v = vagrant.Vagrant(quiet_stdout=False)
                v.halt()


class JmxMetrics:
    """
    Uses JMXTerm to get metrics from the Kafka broker
    """

    def __init__(self, connection):
        """
        :param connection: JMX address in form <hostname>:<port>
        """
        connection_timeout = 2

        self.jmxterm = pexpect.spawn("java -jar jmxterm.jar")
        self.jmxterm.expect_exact("$>")  # got prompt, can continue
        self.jmxterm.sendline("open " + connection)
        self.jmxterm.expect_exact("#Connection to " + connection + " is opened", connection_timeout)

    def get_metric(self, bean_type, bean_name, bean_value):
        request = "get -b kafka.server:type=" + bean_type + ",name=" + bean_name + " " + bean_value
        self.jmxterm.sendline(request)
        response_lines = [self.jmxterm.readline(), self.jmxterm.readline()]
        while response_lines and response_lines[-1] != "\r\n":
            try:
                response_lines.append(self.jmxterm.readline())
            except pexpect.exceptions.TIMEOUT:
                print("Response to metric value request timed out.")
                return response_lines

        return response_lines

    def __del__(self):
        self.jmxterm.sendline("quit")
        self.jmxterm.expect_exact("#bye")


class JmxTool:
    def __init__(self, build_dir, host, topic="system_test_topic", metrics="broker"):
        mbean1 = "'kafka.server:type=BrokerTopicMetrics,name=BytesInPerSec,topic=" + topic + "' "
        mbean2 = "'kafka.server:type=BrokerTopicMetrics,name=BytesOutPerSec,topic=" + topic + "' "
        attributes = "OneMinuteRate "

        if metrics is "cpu":
            mbean1 = "'java.lang:type=OperatingSystem' "
            mbean2 = None
            attributes = "ProcessCpuLoad "  # SystemCpuLoad
        elif metrics is "memory":
            mbean1 = "'java.lang:type=Memory' "
            mbean2 = None
            attributes = "HeapMemoryUsage "

        bash_script = "bash " + os.path.join(build_dir, "system_tests", "kafka_2.11-0.9.0.1", "bin",
                                             "kafka-run-class.sh")

        command = bash_script + (" kafka.tools.JmxTool "
                                 "--object-name " + mbean1)
        if mbean2:
            command = command + "--object-name " + mbean2

        command = (command + "--jmx-url "
                             "service:jmx:rmi:///jndi/rmi://" + host +
                   "/jmxrmi "
                   "--attributes " + attributes +
                   "--reporting-interval 2000")
        print(command)
        self.jmxtool = pexpect.spawn(command)

    def get_output(self):
        # Get all output
        result = ""
        try:
            for n in range(1, 10):
                result = result + self.jmxtool.read_nonblocking(size=16777216, timeout=2)
        except:
            print("Error reading from JmxTool, buffer too small?")
        return result

    def __del__(self):
        self.jmxtool.close(force=True)
