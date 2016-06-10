#ifndef ISIS_NEXUS_STREAMER_KAFKAEVENTPUBLISHER_H
#define ISIS_NEXUS_STREAMER_KAFKAEVENTPUBLISHER_H

#include <librdkafka/rdkafkacpp.h>
#include <memory>

#include "EventPublisher.h"

class KafkaEventPublisher : public EventPublisher {
public:
  void setUp(const std::string &broker, const std::string &topic) override;
  ~KafkaEventPublisher() { RdKafka::wait_destroyed(5000); };

private:
  std::unique_ptr<RdKafka::Producer> m_producer_ptr;
  std::unique_ptr<RdKafka::Topic> m_topic_ptr;
};

#endif // ISIS_NEXUS_STREAMER_KAFKAEVENTPUBLISHER_H
