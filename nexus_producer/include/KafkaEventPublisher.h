#ifndef ISIS_NEXUS_STREAMER_KAFKAEVENTPUBLISHER_H
#define ISIS_NEXUS_STREAMER_KAFKAEVENTPUBLISHER_H

#include <librdkafka/rdkafkacpp.h>
#include <memory>

#include "EventPublisher.h"

class KafkaEventPublisher : public EventPublisher {
public:
  KafkaEventPublisher() {};
  KafkaEventPublisher(const std::string &compression) : m_compression(compression) {};
  ~KafkaEventPublisher() { RdKafka::wait_destroyed(5000); };

  void setUp(const std::string &broker, const std::string &topic) override;
  void sendMessage(char *buf, size_t messageSize) override;

private:
  std::unique_ptr<RdKafka::Producer> m_producer_ptr;
  std::unique_ptr<RdKafka::Topic> m_topic_ptr;
  std::string m_compression = "";
};

#endif // ISIS_NEXUS_STREAMER_KAFKAEVENTPUBLISHER_H
