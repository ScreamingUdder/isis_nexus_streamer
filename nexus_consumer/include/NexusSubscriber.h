#ifndef ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H

#include <memory>

#include "ProtoEventData.h"
#include "EventSubscriber.h"

class NexusSubscriber {
public:
  NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                  const std::string &brokerAddress,
                  const std::string &streamName, const bool quietMode);

  void listen();
  void decodeMessage(std::shared_ptr<ProtoEventData> eventData,
                     std::string &rawbuf);

private:
  std::shared_ptr<EventSubscriber> m_subscriber;
  bool m_quietMode = false;

  void reportProgress(const float progress);
};

#endif // ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
