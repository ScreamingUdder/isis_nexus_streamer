#include <iostream>

#include "KafkaEventSubscriber.h"

void KafkaEventSubscriber::setUp(const std::string &broker_str,
                                 const std::string &topic_str) {
  std::string error_str;
  const int partition = 0;
  const int start_offset = 0;

  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

  conf->set("metadata.broker.list", broker_str, error_str);

  // Create consumer using accumulated global configuration.
  m_consumer_ptr = std::unique_ptr<RdKafka::Consumer>(
      RdKafka::Consumer::create(conf, error_str));
  if (!m_consumer_ptr.get()) {
    std::cerr << "Failed to create consumer: " << error_str << std::endl;
    exit(1);
  }

  std::cout << "% Created consumer " << m_consumer_ptr->name() << std::endl;

  // Create topic handle.
  m_topic_ptr = std::unique_ptr<RdKafka::Topic>(RdKafka::Topic::create(
      m_consumer_ptr.get(), topic_str, tconf, error_str));
  if (!m_topic_ptr.get()) {
    std::cerr << "Failed to create topic: " << error_str << std::endl;
    exit(1);
  }

  // Start consumer for topic+partition at start offset
  RdKafka::ErrorCode resp =
      m_consumer_ptr->start(m_topic_ptr.get(), partition, start_offset);
  if (resp != RdKafka::ERR_NO_ERROR) {
    std::cerr << "Failed to start consumer: " << RdKafka::err2str(resp)
              << std::endl;
    exit(1);
  }
}