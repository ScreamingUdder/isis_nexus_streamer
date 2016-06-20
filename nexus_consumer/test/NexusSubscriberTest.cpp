#include <gtest/gtest.h>

#include "EventData.h"
#include "MockEventSubscriber.h"
#include "NexusSubscriber.h"

using ::testing::AtLeast;
using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;

class NexusSubscriberTest : public ::testing::Test {

public:
  std::shared_ptr<EventData> createEventData() {
    auto exampleEventData = std::make_shared<EventData>();

    std::vector<uint32_t> detIds = {1, 2, 3, 4};
    std::vector<uint64_t> tofs = {4, 3, 2, 1};
    uint32_t frameNumber = 2;
    uint32_t numberOfFrames = 9;

    exampleEventData->setDetId(detIds);
    exampleEventData->setTof(tofs);
    exampleEventData->setNumberOfFrames(numberOfFrames);
    exampleEventData->setFrameNumber(frameNumber);

    return exampleEventData;
  };
};

TEST_F(NexusSubscriberTest, test_create_subscriber) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);
}

TEST_F(NexusSubscriberTest, test_listen_for_messages_none_received) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);
  ON_CALL(*subscriber.get(), listenForMessage(_)).WillByDefault(Return(false));
  EXPECT_CALL(*subscriber.get(), listenForMessage(_)).Times(1);

  EXPECT_NO_THROW(streamer.listen());
}

TEST_F(NexusSubscriberTest, decode_received_message) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  auto exampleEventData = createEventData();
  std::string rawbuf;
  EXPECT_NO_THROW(exampleEventData->getBufferPointer(rawbuf));

  // Decode the message
  auto receivedEvents = std::make_shared<EventData>();
  NexusSubscriber streamer(subscriber, broker, topic);
  EXPECT_NO_THROW(streamer.decodeMessage(receivedEvents, rawbuf));

  EXPECT_EQ(receivedEvents->getDetId(), exampleEventData->getDetId());
  EXPECT_EQ(receivedEvents->getTof(), exampleEventData->getTof());
  EXPECT_EQ(receivedEvents->getNumberOfFrames(),
            exampleEventData->getNumberOfFrames());
  EXPECT_EQ(receivedEvents->getFrameNumber(),
            exampleEventData->getFrameNumber());
}

TEST_F(NexusSubscriberTest, test_listen_for_messages_one_received) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto exampleEventData = createEventData();
  // Make this the last frame so that listenForMessage will only be called once
  exampleEventData->setNumberOfFrames(9);
  exampleEventData->setFrameNumber(9);
  std::string rawbuf;
  EXPECT_NO_THROW(exampleEventData->getBufferPointer(rawbuf));

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);

  // Should be called exactly once because the message contains the last frame
  EXPECT_CALL(*subscriber.get(), listenForMessage(_))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf), Return(true)));

  EXPECT_NO_THROW(streamer.listen());
}

TEST_F(NexusSubscriberTest, test_listen_for_messages_multiple_received) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  // Make this the second to last frame so that listenForMessage will be called
  // twice
  auto exampleEventData_first = createEventData();
  exampleEventData_first->setNumberOfFrames(9);
  exampleEventData_first->setFrameNumber(8);
  std::string rawbuf_first;
  EXPECT_NO_THROW(exampleEventData_first->getBufferPointer(rawbuf_first));

  // Last frame
  auto exampleEventData_second = createEventData();
  exampleEventData_second->setNumberOfFrames(9);
  exampleEventData_second->setFrameNumber(9);
  std::string rawbuf_second;
  EXPECT_NO_THROW(exampleEventData_second->getBufferPointer(rawbuf_second));

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic);

  // Should be called exactly twice because there are messages containing the
  // last two frames
  EXPECT_CALL(*subscriber.get(), listenForMessage(_))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_first), Return(true)))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_second), Return(true)));

  EXPECT_NO_THROW(streamer.listen());
}