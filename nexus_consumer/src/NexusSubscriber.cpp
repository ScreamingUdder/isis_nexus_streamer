#include <ctime>
#include <iostream>

#include "NexusSubscriber.h"

NexusSubscriber::NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                                 const std::string &brokerAddress,
                                 const std::string &streamName,
                                 const bool quietMode)
    : m_subscriber(subscriber), m_quietMode(quietMode) {
  subscriber->setUp(brokerAddress, streamName);
}

void NexusSubscriber::listen() {
  int32_t frameNumber = 0;
  int32_t numberOfFrames = 2;
  int64_t totalBytesReceived = 0;
  int32_t numberOfmessagesReceived = 0;
  std::string message;
  auto receivedData = std::make_shared<EventData>();
  int numberOfFramesReceived = 0; // to check no messages lost
  int32_t previousFrameNumber = std::numeric_limits<int32_t>::max();
  int messagesPerFrame = 1;

  reportProgress(0.0);

  // frame numbers run from 0 to numberOfFrames-1
  while ((frameNumber < (numberOfFrames - 1)) ||
         (numberOfmessagesReceived % messagesPerFrame) != 0) {
    if (!(m_subscriber->listenForMessage(message)))
      continue;

    decodeMessage(receivedData, message);
    totalBytesReceived += message.size();
    numberOfFrames = receivedData->getNumberOfFrames();
    frameNumber = receivedData->getFrameNumber();
    if (frameNumber != previousFrameNumber) {
      if (frameNumber == 1 && messagesPerFrame == 1)
        messagesPerFrame = numberOfmessagesReceived;
      numberOfFramesReceived++;
      reportProgress(static_cast<float>(frameNumber) /
                     static_cast<float>(numberOfFrames));
    }
    previousFrameNumber = frameNumber;
    numberOfmessagesReceived++;
  }
  reportProgress(1.0);
  std::cout << std::endl
            << "Frames received: " << numberOfFramesReceived << std::endl
            << "Bytes received: " << totalBytesReceived << std::endl
            << "Number of messages received: " << numberOfmessagesReceived
            << std::endl;
}

void NexusSubscriber::decodeMessage(std::shared_ptr<EventData> eventData,
                                    const std::string &rawbuf) {
  eventData->decodeMessage(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
}

/**
 * Display a progress bar
 *
 * @param progress - progress between 0 (starting) and 1 (complete)
 */
void NexusSubscriber::reportProgress(const float progress) {
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
