#include <iostream>

#include "NexusPublisher.h"

/**
 * Create an object responsible for the main business logic of the software. It
 * manages reading data from the NeXus file and publishing it to the data
 * stream.
 *
 * @param publisher - the publisher which provides methods to publish the data
 * to a data stream
 * @param brokerAddress - the IP or hostname of the broker for the data stream
 * @param streamName - the name of the data stream, called the topic in the case
 * of a Kafka publisher
 * @param filename - the full path of a NeXus file
 * @return - a NeXusPublisher object, call streamData() on it to start streaming
 * data
 */
NexusPublisher::NexusPublisher(std::shared_ptr<EventPublisher> publisher,
                               const std::string &brokerAddress,
                               const std::string &streamName,
                               const std::string &filename,
                               const bool quietMode)
    : m_publisher(publisher),
      m_fileReader(std::make_shared<NexusFileReader>(filename)),
      m_quietMode(quietMode) {
  publisher->setUp(brokerAddress, streamName);
}

/**
 * For a given frame number, reads the data from file and stores it in an
 * EventData object
 *
 * @param frameNumber - the number of the frame for which to construct a message
 * @return - an object containing the data from the specified frame
 */
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

  // std::cout << "Creating message: frame " << frameNumber << "/"
  //          << (m_fileReader->getNumberOfFrames() - 1) << ",";

  return eventData;
}

/**
 * Start streaming all the data from the file
 */
void NexusPublisher::streamData() {
  std::string rawbuf;
  // frame numbers run from 0 to numberOfFrames-1
  reportProgress(0.0);
  int64_t totalBytesSent = 0;
  const auto numberOfFrames = m_fileReader->getNumberOfFrames();
  for (size_t frameNumber = 0; frameNumber < numberOfFrames; frameNumber++) {
    createAndSendMessage(rawbuf, frameNumber);
    reportProgress(static_cast<float>(frameNumber) /
                   static_cast<float>(numberOfFrames));
    totalBytesSent += rawbuf.size();
  }
  reportProgress(1.0);
  std::cout << std::endl
            << "Frames sent: " << m_fileReader->getNumberOfFrames()
            << std::endl
            << "Bytes sent: " << totalBytesSent << std::endl;
}

/**
 * Using Google Flatbuffers, create a message for the specifed frame and store
 * it in the provided buffer
 *
 * @param rawbuf - a buffer for the message
 * @param frameNumber - the number of the frame for which data will be sent
 */
void NexusPublisher::createAndSendMessage(std::string &rawbuf,
                                          size_t frameNumber) {
  auto messageData = createMessageData(frameNumber);
  auto buffer_uptr = messageData->getBufferPointer(rawbuf);
  m_publisher->sendMessage(reinterpret_cast<char *>(buffer_uptr.get()),
                           messageData->getBufferSize());
}

/**
 * Display a progress bar
 *
 * @param progress - progress between 0 (starting) and 1 (complete)
 */
void NexusPublisher::reportProgress(const float progress) {
  if (!m_quietMode) {
    const int barWidth = 70;
    std::cout << "[";
    auto pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
  }
}
