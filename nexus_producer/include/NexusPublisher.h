#ifndef ISIS_NEXUS_STREAMER_NEXUSPUBLISHER_H
#define ISIS_NEXUS_STREAMER_NEXUSPUBLISHER_H

#include <memory>

#include "../../event_data/include/EventData.h"
#include "EventPublisher.h"
#include "../../nexus_file_reader/include/NexusFileReader.h"

class NexusPublisher {
public:
  NexusPublisher(std::shared_ptr<EventPublisher> publisher,
                 const std::string &brokerAddress,
                 const std::string &streamName, const std::string &filename,
                 const bool quietMode);
  std::vector<std::shared_ptr<EventData>>
  createMessageData(hsize_t frameNumber, const int messagesPerFrame);
  void streamData(const int messagesPerFrame);

private:
  int64_t createAndSendMessage(std::string &rawbuf, size_t frameNumber,
                            const int messagesPerFrame);
  void reportProgress(const float progress);

  std::shared_ptr<EventPublisher> m_publisher;
  std::shared_ptr<NexusFileReader> m_fileReader;
  bool m_quietMode = false;
  uint64_t m_messageID = 0;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSPUBLISHER_H
