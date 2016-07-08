#include <ctime>
#include <iostream>

#include "NexusSubscriber.h"

NexusSubscriber::NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                                 const std::string &brokerAddress,
                                 const std::string &streamName)
    : m_subscriber(subscriber) {
  subscriber->setUp(brokerAddress, streamName);
}

void NexusSubscriber::listen() {
  int32_t frameNumber = 0;
  int32_t numberOfFrames = 2;
  std::string message;
  auto receivedData = std::make_shared<EventData>();
  int numberOfFramesReceived = 0; // to check no messages lost
  // frame numbers run from 0 to numberOfFrames-1
  while (frameNumber < (numberOfFrames - 1)) {
    if (!(m_subscriber->listenForMessage(message)))
      continue;
    decodeMessage(receivedData, message);
    frameNumber = receivedData->getFrameNumber();
    numberOfFrames = receivedData->getNumberOfFrames();
    numberOfFramesReceived++;
  }
  std::cout << "Counted number of frames received: " << numberOfFramesReceived << std::endl;
}

void NexusSubscriber::decodeMessage(std::shared_ptr<EventData> eventData,
                                    const std::string &rawbuf) {
  eventData->decodeMessage(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
}
