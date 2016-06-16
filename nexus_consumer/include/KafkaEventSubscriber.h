#ifndef ISIS_NEXUS_STREAMER_KAFKAEVENTSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_KAFKAEVENTSUBSCRIBER_H

#include <librdkafka/rdkafkacpp.h>
#include <memory>

#include "EventSubscriber.h"

class KafkaEventSubscriber : public EventSubscriber {
public:
  void setUp(const std::string &broker, const std::string &topic) override;
  ~KafkaEventPublisher() { RdKafka::wait_destroyed(5000); };

private:
  std::unique_ptr<RdKafka::Consumer> m_consumer_ptr;
  std::unique_ptr<RdKafka::Topic> m_topic_ptr;
};

#endif //ISIS_NEXUS_STREAMER_KAFKAEVENTSUBSCRIBER_H
