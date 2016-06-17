#ifndef ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H

#include <memory>

#include "EventSubscriber.h"

class NexusSubscriber {
public:
  NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                  const std::string &brokerAddress,
                  const std::string &streamName);

  void listen();

private:
  std::shared_ptr<EventSubscriber> m_subscriber;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
