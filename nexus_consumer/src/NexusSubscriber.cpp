#include "NexusSubscriber.h"

NexusSubscriber::NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                                 const std::string &brokerAddress,
                                 const std::string &streamName)
    : m_subscriber(subscriber) {
  subscriber->setUp(brokerAddress, streamName);
}
