#ifndef ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H

#include <memory>

#include "EventData.h"
#include "EventSubscriber.h"
#include "NexusFileWriter.h"

class NexusSubscriber {
public:
  NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                  const std::string &brokerAddress,
                  const std::string &streamName, const bool quietMode,
                  const std::string &filename);

  void listen();
  void decodeMessage(std::shared_ptr<EventData> eventData,
                     const std::string &rawbuf);

private:
  std::shared_ptr<EventSubscriber> m_subscriber;
  std::shared_ptr<NexusFileWriter> m_filewriter;
  bool m_quietMode = false;

  void reportProgress(const float progress);
};

#endif // ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
