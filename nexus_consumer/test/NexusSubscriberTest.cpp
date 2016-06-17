#include <gtest/gtest.h>

#include "MockEventSubscriber.h"
#include "NexusSubscriber.h"

using ::testing::AtLeast;
using ::testing::_;
using ::testing::Return;

class NexusSubscriberTest : public ::testing::Test {};

TEST(NexusSubscriberTest, test_create_subscriber) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);
}

TEST(NexusSubscriberTest, test_listen_for_messages) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);
  ON_CALL(*subscriber.get(), listenForMessage(_)).WillByDefault(Return(false));
  EXPECT_CALL(*subscriber.get(), listenForMessage(_)).Times(AtLeast(1));

  EXPECT_NO_THROW(streamer.listen());
}
