#include <iostream>

#include "KafkaEventPublisher.h"

/**
 * Set up the configuration for the publisher and initialise it
 *
 * @param broker_str - the IP or hostname of the broker
 * @param topic_str - the name of the datastream (topic) to publish the data to
 */
void KafkaEventPublisher::setUp(const std::string &broker_str,
                                const std::string &topic_str) {

  std::cout << "Setting up Kafka producer" << std::endl;

  std::string error_str;

  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

  conf->set("metadata.broker.list", broker_str, error_str);
  conf->set("message.max.bytes", "10000000", error_str);
  conf->set("fetch.message.max.bytes", "10000000", error_str);
  conf->set("replica.fetch.max.bytes", "10000000", error_str);
  //if (conf->set("compression.codec", "snappy", error_str) !=
  //    RdKafka::Conf::CONF_OK) {
  //  std::cerr << error_str << std::endl;
  //  exit(1);
  //}

  // Create producer
  m_producer_ptr = std::unique_ptr<RdKafka::Producer>(
      RdKafka::Producer::create(conf, error_str));
  if (!m_producer_ptr.get()) {
    std::cerr << "Failed to create producer: " << error_str << std::endl;
    exit(1);
  }

  // Create topic handle
  m_topic_ptr = std::unique_ptr<RdKafka::Topic>(RdKafka::Topic::create(
      m_producer_ptr.get(), topic_str, tconf, error_str));
  if (!m_topic_ptr.get()) {
    std::cerr << "Failed to create topic: " << error_str << std::endl;
    exit(1);
  }
}

/**
 * Publish the provided message to the datastream
 *
 * @param buf - pointer to the message buffer
 * @param messageSize - the size of the message in bytes
 */
void KafkaEventPublisher::sendMessage(char *buf, size_t messageSize) {
  // using -1 as the partition number will cause rdkafka to distribute messages
  // across multiple partitions to load balance (if the topic has multiple
  // partitions)
  auto resp = m_producer_ptr->produce(
      m_topic_ptr.get(), -1, RdKafka::Producer::RK_MSG_COPY, buf, messageSize,
      NULL, NULL);

  if (resp != RdKafka::ERR_NO_ERROR) {
    std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;
    std::cerr << "message size was " << messageSize << std::endl;
  }

  m_producer_ptr->poll(0);
}
