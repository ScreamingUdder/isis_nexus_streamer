#include "NexusStreamer.h"

NexusStreamer::NexusStreamer(std::shared_ptr<EventPublisher> publisher) {
  publisher->createConfiguration("something1", "something2");
}
