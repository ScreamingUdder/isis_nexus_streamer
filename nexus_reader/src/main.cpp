#include "KafkaEventPublisher.h"
#include "NexusStreamer.h"

int main() {

  auto publisher = std::make_shared<KafkaEventPublisher>();
  NexusStreamer streamer(publisher, "sakura", "test_topic");

  return 0;
}
