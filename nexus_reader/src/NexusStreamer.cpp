#include "NexusStreamer.h"

NexusStreamer::NexusStreamer(std::shared_ptr<EventPublisher> publisher) {
  publisher->setUp("something1", "something2");
}
