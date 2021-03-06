#ifndef ISIS_NEXUS_STREAMER_KAFKAEVENTSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_KAFKAEVENTSUBSCRIBER_H

#include <librdkafka/rdkafkacpp.h>
#include <memory>

#include "EventSubscriber.h"

class KafkaEventSubscriber : public EventSubscriber {
public:
  void setUp(const std::string &broker, const std::string &topic) override;
  bool listenForMessage(std::string &message) override;
  bool messageConsume(RdKafka::Message *message, std::string &msg);
  ~KafkaEventSubscriber();

private:
  std::unique_ptr<RdKafka::KafkaConsumer> m_consumer_ptr;
};

#endif //ISIS_NEXUS_STREAMER_KAFKAEVENTSUBSCRIBER_H
