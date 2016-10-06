#include <iostream>

#include "KafkaEventSubscriber.h"

namespace {
const int static PARTITION = 0;
}

KafkaEventSubscriber::~KafkaEventSubscriber() {
  m_consumer_ptr->close();
  // Wait for RdKafka to decommission, avoids complaints of memory leak from
  // valgrind etc.
  RdKafka::wait_destroyed(5000);
}

void KafkaEventSubscriber::setUp(const std::string &broker_str,
                                 const std::string &topic_str) {
  std::string error_str;

  auto conf = std::unique_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

  conf->set("metadata.broker.list", broker_str, error_str);
  conf->set("message.max.bytes", "10000000", error_str);
  conf->set("fetch.message.max.bytes", "10000000", error_str);
  conf->set("replica.fetch.max.bytes", "10000000", error_str);
  conf->set("group.id", "nexus_stream_consumer", error_str);
  conf->set("enable.auto.commit", "false", error_str);
  conf->set("enable.auto.offset.store", "false", error_str);
  conf->set("offset.store.method", "none", error_str);
  conf->set("auto.offset.reset", "largest", error_str);

  // Create consumer using accumulated global configuration.
  m_consumer_ptr = std::unique_ptr<RdKafka::KafkaConsumer>(
      RdKafka::KafkaConsumer::create(conf.get(), error_str));
  if (!m_consumer_ptr.get()) {
    std::cerr << "Failed to create consumer: " << error_str << std::endl;
    exit(1);
  }

  std::cout << "% Created consumer " << m_consumer_ptr->name() << std::endl;

  // Start consumer for topic+partition at start offset
  std::vector<std::string> topics = {topic_str};
  RdKafka::ErrorCode resp = m_consumer_ptr->subscribe(topics);
  if (resp != RdKafka::ERR_NO_ERROR) {
    std::cerr << "Failed to start consumer: " << RdKafka::err2str(resp)
              << std::endl;
    exit(1);
  }
}

bool KafkaEventSubscriber::listenForMessage(std::string &message) {
  RdKafka::Message *msg =
      m_consumer_ptr->consume(1000);
  bool success = messageConsume(msg, message);
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
