#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#endif
#include <iostream>

#include "KafkaEventPublisher.h"
#include "NexusStreamer.h"

int main(int argc, char **argv) {

  int opt;
  std::string filename;
  std::string broker = "sakura";
  std::string topic = "test_topic";

  while ((opt = getopt(argc, argv, "f:b:t")) != -1) {
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

    default:
      goto usage;
    }
  }

  if (filename.empty()) {
    usage:
    fprintf(stderr,
            "Usage: %s -f <filepath> "
                "[-b <host:port>] "
                "[-t <topic_name>]"
                "\n", argv[0]);
    exit(1);
  }

  auto publisher = std::make_shared<KafkaEventPublisher>();
  NexusStreamer streamer(publisher, broker, topic, filename);

  return 0;
}
