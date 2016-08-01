#ifndef ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H

#include <memory>

#include "Compatibility.h"
#include "EventData.h"
#include "EventSubscriber.h"

class DLLExport NexusSubscriber {
public:
  NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                  const std::string &brokerAddress,
                  const std::string &streamName, const bool quietMode);

  void listen();
  void decodeMessage(std::shared_ptr<EventData> eventData,
                     const std::string &rawbuf);

private:
  std::shared_ptr<EventSubscriber> m_subscriber;
  bool m_quietMode = false;

  void reportProgress(const float progress);
};

#endif // ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
