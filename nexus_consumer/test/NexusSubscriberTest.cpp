#include <gtest/gtest.h>

#include "MockEventSubscriber.h"
#include "NexusSubscriber.h"

using ::testing::AtLeast;

class NexusSubscriberTest : public ::testing::Test {};

TEST(NexusSubscriberTest, test_create_subscriber) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);
}
