#include "NexusPublisher.h"

NexusPublisher::NexusPublisher(std::shared_ptr<EventPublisher> publisher,
                               const std::string &brokerAddress,
                               const std::string &streamName,
                               const std::string &filename)
    : m_publisher(publisher),
      m_fileReader(std::make_shared<NexusFileReader>(filename)) {
  publisher->setUp(brokerAddress, streamName);
}

std::shared_ptr<EventData>
NexusPublisher::createMessageData(hsize_t frameNumber) {
  std::vector<uint32_t> detIds;
  m_fileReader->getEventDetIds(detIds, frameNumber);
  std::vector<uint64_t> tofs;
  m_fileReader->getEventTofs(tofs, frameNumber);

  auto eventData = std::make_shared<EventData>();
  eventData->setDetId(detIds);
  eventData->setTof(tofs);
  eventData->setNumberOfFrames(
      static_cast<uint32_t>(m_fileReader->getNumberOfFrames()));
  eventData->setFrameNumber(static_cast<uint32_t>(frameNumber));

  return eventData;
}

void NexusPublisher::streamData() {
  std::string rawbuf;
  for (size_t frameNumber = 0; frameNumber < m_fileReader->getNumberOfFrames();
       frameNumber++) {
    createAndSendMessage(rawbuf, frameNumber);
  }
}

void NexusPublisher::createAndSendMessage(std::string &rawbuf,
                                          size_t frameNumber) {
  auto messageData = createMessageData(frameNumber);
  m_publisher->sendMessage(
      reinterpret_cast<char *>(messageData->getBufferPointer(rawbuf).get()),
      messageData->getBufferSize());
}
