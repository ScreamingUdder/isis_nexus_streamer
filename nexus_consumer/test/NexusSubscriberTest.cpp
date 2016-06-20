#include <gtest/gtest.h>

#include "MockEventSubscriber.h"
#include "NexusSubscriber.h"
#include "EventData.h"

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

TEST(NexusSubscriberTest, test_listen_for_messages_none_received) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);
  ON_CALL(*subscriber.get(), listenForMessage(_)).WillByDefault(Return(false));
  EXPECT_CALL(*subscriber.get(), listenForMessage(_)).Times(1);

  EXPECT_NO_THROW(streamer.listen());
}

TEST(NexusSubscriberTest, decode_received_message) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  // Construct a flatbuffer message with numberOfFrames=1 so that listen loop terminates
  auto events = EventData();

  std::vector<uint32_t> detIds = {1, 2, 3, 4};
  std::vector<uint64_t> tofs = {4, 3, 2, 1};
  uint32_t frameNumber = 2;
  uint32_t numberOfFrames = 9;

  EXPECT_NO_THROW(events.setDetId(detIds));
  EXPECT_NO_THROW(events.setTof(tofs));
  EXPECT_NO_THROW(events.setNumberOfFrames(numberOfFrames));
  EXPECT_NO_THROW(events.setFrameNumber(frameNumber));

  std::string rawbuf;
  EXPECT_NO_THROW(events.getBufferPointer(rawbuf));

  // Decode the message
  auto receivedEvents = std::make_shared<EventData>();
  NexusSubscriber streamer(subscriber, broker, topic);
  EXPECT_NO_THROW(streamer.decodeMessage(receivedEvents, rawbuf));

  EXPECT_EQ(receivedEvents->getDetId(), detIds);
  EXPECT_EQ(receivedEvents->getTof(), tofs);
  EXPECT_EQ(receivedEvents->getNumberOfFrames(), numberOfFrames);
  EXPECT_EQ(receivedEvents->getFrameNumber(), frameNumber);
}
