#ifndef ISIS_NEXUS_STREAMER_NEXUSPUBLISHER_H
#define ISIS_NEXUS_STREAMER_NEXUSPUBLISHER_H

#include <memory>

#include "../../event_data/include/EventData.h"
#include "EventPublisher.h"
#include "NexusFileReader.h"

class NexusPublisher {
public:
  NexusPublisher(std::shared_ptr<EventPublisher> publisher,
                 const std::string &brokerAddress,
                 const std::string &streamName, const std::string &filename,
                 const bool quietMode);
  std::vector<std::shared_ptr<EventData>>
  createMessageData(hsize_t frameNumber, int messagesPerFrame);
  void streamData();

private:
  void createAndSendMessage(std::string &rawbuf, size_t frameNumber);
  void reportProgress(const float progress);

  std::shared_ptr<EventPublisher> m_publisher;
  std::shared_ptr<NexusFileReader> m_fileReader;
  bool m_quietMode = false;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSPUBLISHER_H
