#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#endif
#include <iostream>

#include "KafkaEventSubscriber.h"
#include "NexusSubscriber.h"

int main(int argc, char **argv) {

  int opt;
  std::string broker = "sakura";
  std::string topic = "test_topic";
  std::string filename = "";
  bool quietMode = false;

  while ((opt = getopt(argc, argv, "b:t:f:q")) != -1) {
    switch (opt) {

    case 'b':
      broker = optarg;
      break;

    case 't':
      topic = optarg;
      break;

    case 'f':
      filename = optarg;
      break;

    case 'q':
      quietMode = true;
      break;

    default:
      fprintf(stderr, "Usage: %s \n"
                      "[-b <host>]    Hostname of a broker in the Kafka cluster\n"
                      "[-t <topic_name>]    Name of the topic to subscribe to\n"
                      "[-f <filepath>]    Write the received data to a NeXus file with this full path\n"
                      "[-q]    Quiet mode, make the client less chatty\n"
                      "\n",
              argv[0]);
      exit(1);
    }
  }

  auto subscriber = std::make_shared<KafkaEventSubscriber>();
  NexusSubscriber streamer(subscriber, broker, topic, quietMode, filename);
  streamer.listen();

  return 0;
}
