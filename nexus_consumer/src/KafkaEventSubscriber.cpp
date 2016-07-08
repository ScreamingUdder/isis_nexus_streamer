#include <iostream>

#include "KafkaEventSubscriber.h"

namespace {
const int static PARTITION = 0;
}

KafkaEventSubscriber::~KafkaEventSubscriber() {
  m_consumer_ptr->stop(m_topic_ptr.get(), PARTITION);
  // rdkafka example polls after stop() is called,
  // not sure why but doing the same here for now
  m_consumer_ptr->poll(1000);
  // Wait for RdKafka to decommission, avoids complaints of memory leak from
  // valgrind etc.
  RdKafka::wait_destroyed(5000);
}

void KafkaEventSubscriber::setUp(const std::string &broker_str,
                                 const std::string &topic_str) {
  std::string error_str;

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
      m_consumer_ptr->start(m_topic_ptr.get(), PARTITION, RdKafka::Topic::OFFSET_END);
  if (resp != RdKafka::ERR_NO_ERROR) {
    std::cerr << "Failed to start consumer: " << RdKafka::err2str(resp)
              << std::endl;
    exit(1);
  }
}

bool KafkaEventSubscriber::listenForMessage(std::string &message) {
  RdKafka::Message *msg =
      m_consumer_ptr->consume(m_topic_ptr.get(), PARTITION, 1000);
  bool success = messageConsume(msg, message);
  m_consumer_ptr->poll(0);
  delete msg;
  return success;
}

bool KafkaEventSubscriber::messageConsume(RdKafka::Message *msg,
                                          std::string &message) {
  switch (msg->err()) {
  case RdKafka::ERR__TIMED_OUT:
    break;

  case RdKafka::ERR_NO_ERROR:
    /* Real message */
    if (msg->len() == 0) {
      std::cout << "Warning: message received had 0 length payload!"
                << std::endl;
      return false;
    }
    if (msg->key()) {
      std::cout << "Key: " << *msg->key() << std::endl;
    }
    message.assign(static_cast<const char *>(msg->payload()),
                   static_cast<int>(msg->len()));
    return true;

  case RdKafka::ERR__PARTITION_EOF:
    return false;

  default:
    /* Errors */
    std::cerr << "Consume failed: " << msg->errstr() << std::endl;
  }
  return false;
}
