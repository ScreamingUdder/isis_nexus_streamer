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
  int32_t numberOfFrames = 1;
  std::string message;
  auto receivedData = std::make_shared<EventData>();
  while (frameNumber < numberOfFrames) {
    if (!(m_subscriber->listenForMessage(message)))
      continue;
    decodeMessage(receivedData, message);
    frameNumber = receivedData->getFrameNumber();
    numberOfFrames = receivedData->getNumberOfFrames();
    std::cout << "Received message: " << receivedData->getNumberOfEvents()
              << " events, frame " << frameNumber << "/" << numberOfFrames
              << std::endl;
  }
}

void NexusSubscriber::decodeMessage(std::shared_ptr<EventData> eventData,
                                    const std::string &rawbuf) {
  eventData->decodeMessage(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
}
