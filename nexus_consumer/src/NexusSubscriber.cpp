#include <ctime>

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
  while (frameNumber < numberOfFrames) {
    if (!(m_subscriber->listenForMessage(message)))
      break;
    // TODO do something with the message

  }
}
