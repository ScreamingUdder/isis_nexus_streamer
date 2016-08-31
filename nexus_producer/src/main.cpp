#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#endif
#include <iostream>

#include "KafkaEventPublisher.h"
#include "NexusPublisher.h"

int main(int argc, char **argv) {

  int opt;
  std::string filename;
  std::string broker = "sakura";
  std::string topic = "test_topic";
  std::string compression = "";
  bool quietMode = false;
  bool randomMode = false;
  int maxEventsPerFramePart = 200;

  while ((opt = getopt(argc, argv, "f:b:t:c:m:qr")) != -1) {
    switch (opt) {

    case 'f':
      filename = optarg;
      break;

    case 'b':
      broker = optarg;
      break;

    case 't':
      topic = optarg;
      break;

    case 'c':
      compression = optarg;
      break;

    case 'm':
      maxEventsPerFramePart = std::stoi(optarg);
      break;

    case 'q':
      quietMode = true;
      break;

    case 'r':
      randomMode = true;
      break;

    default:
      goto usage;
    }
  }

  if (filename.empty()) {
  usage:
    fprintf(stderr, "Usage: %s -f <filepath>    NeXus filename including full path\n"
                    "[-b <host>]    hostname of a broker in the Kafka cluster\n"
                    "[-t <topic_name>]    name of the topic to publish to\n"
                    "[-m <max_events_per_message>]   Maximum number of events to send "
                    "in a single message, default is 200\n"
                    "[-q]    quiet mode, make publisher less chatty\n"
                    "[-r]    random mode, serve messages within each frame in a random order\n"
                    "\n",
            argv[0]);
    exit(1);
  }

  auto publisher = std::make_shared<KafkaEventPublisher>(compression);
  NexusPublisher streamer(publisher, broker, topic, filename, quietMode, randomMode);
  streamer.streamData(maxEventsPerFramePart);

  return 0;
}
