#include "NexusStreamer.h"

NexusStreamer::NexusStreamer(std::shared_ptr<EventPublisher> publisher,
                             const std::string &brokerAddress,
                             const std::string &streamName,
                             const std::string &filename)
    : m_publisher(publisher),
      m_fileReader(std::make_shared<NexusFileReader>(filename)) {
  publisher->setUp(brokerAddress, streamName);
}

std::shared_ptr<EventData>
NexusStreamer::createMessageData(hsize_t frameNumber) {
  std::vector<uint32_t> detIds;
  m_fileReader->getEventDetIds(detIds, frameNumber);
  std::vector<uint64_t> tofs;
  m_fileReader->getEventTofs(tofs, frameNumber);

  auto eventData = std::make_shared<EventData>();
  eventData->setDetId(detIds);
  eventData->setTof(tofs);

  return eventData;
}

void NexusStreamer::streamData() {
  std::string rawbuf;
  for (size_t frameNumber = 0; frameNumber < m_fileReader->getNumberOfFrames();
       frameNumber++) {
    createAndSendMessage(rawbuf, frameNumber);
  }
}

void NexusStreamer::streamData(size_t maxFrameNumber) {
  std::string rawbuf;
  size_t framesAvailable = m_fileReader->getNumberOfFrames();
  if (maxFrameNumber > framesAvailable) {
    maxFrameNumber = framesAvailable;
  }
  for (size_t frameNumber = 0; frameNumber < maxFrameNumber; frameNumber++) {
    createAndSendMessage(rawbuf, frameNumber);
  }
}

void NexusStreamer::createAndSendMessage(std::string &rawbuf,
                                         size_t frameNumber) {
  auto messageData = createMessageData(frameNumber);
  m_publisher->sendMessage(
      reinterpret_cast<char *>(messageData->getBufferPointer(rawbuf).get()),
      messageData->getBufferSize());
}
