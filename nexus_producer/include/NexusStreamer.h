#ifndef ISIS_NEXUS_STREAMER_NEXUSSTREAMER_H
#define ISIS_NEXUS_STREAMER_NEXUSSTREAMER_H

#include <memory>

#include "EventData.h"
#include "EventPublisher.h"
#include "NexusFileReader.h"

class NexusStreamer {
public:
  NexusStreamer(std::shared_ptr<EventPublisher> publisher,
                const std::string &brokerAddress, const std::string &streamName,
                const std::string &filename);
  std::shared_ptr<EventData> createMessageData(hsize_t frameNumber);
  void streamData();

private:
  void createAndSendMessage(std::string &rawbuf, size_t frameNumber);

  std::shared_ptr<EventPublisher> m_publisher;
  std::shared_ptr<NexusFileReader> m_fileReader;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSSTREAMER_H
