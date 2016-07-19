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
  bool quietMode = false;

  while ((opt = getopt(argc, argv, "b:t:q")) != -1) {
    switch (opt) {

    case 'b':
      broker = optarg;
      break;

    case 't':
      topic = optarg;
      break;

    case 'q':
      quietMode = true;
      break;

    default:
      fprintf(stderr, "Usage: %s "
                      "[-b <host>] "
                      "[-t <topic_name>]"
                      "\n",
              argv[0]);
      exit(1);
    }
  }

  auto subscriber = std::make_shared<KafkaEventSubscriber>();
  NexusSubscriber streamer(subscriber, broker, topic, quietMode);
  streamer.listen();

  return 0;
}
