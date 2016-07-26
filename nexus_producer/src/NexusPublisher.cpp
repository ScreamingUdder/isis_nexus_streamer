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
 * For a given frame number, reads the data from file and stores them in
 * messagesPerFrame EventData objects
 *
 * @param frameNumber - the number of the frame for which to construct a message
 * @return - an object containing the data from the specified frame
 */
std::vector<std::shared_ptr<EventData>>
NexusPublisher::createMessageData(hsize_t frameNumber,
                                  const int messagesPerFrame) {
  std::vector<std::shared_ptr<EventData>> eventDataVector;

  std::vector<uint32_t> detIds;
  m_fileReader->getEventDetIds(detIds, frameNumber);
  std::vector<uint64_t> tofs;
  m_fileReader->getEventTofs(tofs, frameNumber);

  uint32_t eventsPerMessage =
      static_cast<uint32_t>(ceil(static_cast<double>(detIds.size()) /
                                 static_cast<double>(messagesPerFrame)));
  for (int messageNumber = 0; messageNumber < messagesPerFrame;
       messageNumber++) {

    auto upToDetId = detIds.begin() + ((messageNumber + 1) * eventsPerMessage);
    auto upToTof = tofs.begin() + ((messageNumber + 1) * eventsPerMessage);

    // The last message of the frame will contain any remaining events
    if (messageNumber == (messagesPerFrame - 1)) {
      upToDetId = detIds.end();
      upToTof = tofs.end();
    }

    std::vector<uint32_t> detIdsCurrentMessage(
        detIds.begin() + (messageNumber * eventsPerMessage), upToDetId);
    std::vector<uint64_t> tofsCurrentMessage(
        tofs.begin() + (messageNumber * eventsPerMessage), upToTof);

    auto eventData = std::make_shared<EventData>();
    eventData->setDetId(detIdsCurrentMessage);
    eventData->setTof(tofsCurrentMessage);
    eventData->setNumberOfFrames(
        static_cast<uint32_t>(m_fileReader->getNumberOfFrames()));
    eventData->setFrameNumber(static_cast<uint32_t>(frameNumber));

    eventDataVector.push_back(eventData);
  }

  return eventDataVector;
}

/**
 * Start streaming all the data from the file
 */
void NexusPublisher::streamData(const int messagesPerFrame) {
  std::string rawbuf;
  // frame numbers run from 0 to numberOfFrames-1
  reportProgress(0.0);
  int64_t totalBytesSent = 0;
  const auto numberOfFrames = m_fileReader->getNumberOfFrames();
  for (size_t frameNumber = 0; frameNumber < numberOfFrames; frameNumber++) {
    totalBytesSent +=
        createAndSendMessage(rawbuf, frameNumber, messagesPerFrame);
    reportProgress(static_cast<float>(frameNumber) /
                   static_cast<float>(numberOfFrames));
  }
  reportProgress(1.0);
  std::cout << std::endl
            << "Frames sent: " << m_fileReader->getNumberOfFrames() << std::endl
            << "Bytes sent: " << totalBytesSent << std::endl
            << "Average message size: "
            << totalBytesSent / (messagesPerFrame * numberOfFrames * 1000)
            << " kB"
            << std::endl;
}

/**
 * Using Google Flatbuffers, create a message for the specifed frame and store
 * it in the provided buffer
 *
 * @param rawbuf - a buffer for the message
 * @param frameNumber - the number of the frame for which data will be sent
 */
int64_t NexusPublisher::createAndSendMessage(std::string &rawbuf,
                                             size_t frameNumber,
                                             const int messagesPerFrame) {
  auto messageData = createMessageData(frameNumber, messagesPerFrame);
  int64_t dataSize = 0;
  for (const auto &message : messageData) {
    auto buffer_uptr = message->getBufferPointer(rawbuf);
    m_publisher->sendMessage(reinterpret_cast<char *>(buffer_uptr.get()),
                             message->getBufferSize());
    dataSize += rawbuf.size();
  }
  return dataSize;
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
