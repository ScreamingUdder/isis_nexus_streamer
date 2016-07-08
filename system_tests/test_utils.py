import os
import subprocess
import pexpect


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
        self._return_code = self.p.returncode

        if self._return_code < 0:
            self.terminated_by_signal = True
            self.exited = False
            self.signal = -self._return_code
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
        self.jmxterm.sendline("get -b kafka.server:type=" + bean_type + ",name=" + bean_name + " " + bean_value)
        response_lines = [self.jmxterm.readline(), self.jmxterm.readline()]
        while response_lines and response_lines[-1] != "\r\n":
            response_lines.append(self.jmxterm.readline())

        return response_lines

    def __del__(self):
        self.jmxterm.sendline("quit")
