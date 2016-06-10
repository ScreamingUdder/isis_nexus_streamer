#ifndef ISIS_NEXUS_STREAMER_NEXUSSTREAMER_H
#define ISIS_NEXUS_STREAMER_NEXUSSTREAMER_H

#include <memory>
#include "EventPublisher.h"

class NexusStreamer {
public:
  NexusStreamer(std::shared_ptr<EventPublisher> publisher);
};

#endif //ISIS_NEXUS_STREAMER_NEXUSSTREAMER_H
