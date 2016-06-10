#include "NexusStreamer.h"
#include "KafkaEventPublisher.h"

int main() {

  auto publisher = std::make_shared<KafkaEventPublisher>();
  NexusStreamer streamer(publisher);

  return 0; }
