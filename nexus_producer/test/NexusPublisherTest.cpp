#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "MockEventPublisher.h"
#include "NexusPublisher.h"

using ::testing::AtLeast;
using ::testing::_;

class NexusPublisherTest : public ::testing::Test {};

TEST(NexusPublisherTest, test_create_streamer) {
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();
  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusPublisher streamer(publisher, broker, topic,
                          testDataPath + "SANS_test_reduced.hdf5", false,
                          false);
}

TEST(NexusPublisherTest, test_create_streamer_quiet) {
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();
  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusPublisher streamer(publisher, broker, topic,
                          testDataPath + "SANS_test_reduced.hdf5", true, false);
}

TEST(NexusPublisherTest, test_create_message_data) {
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";
  uint64_t messageID = 0;

  auto publisher = std::make_shared<MockEventPublisher>();

  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusPublisher streamer(publisher, broker, topic,
                          testDataPath + "SANS_test_reduced.hdf5", true, true);
  auto eventData = streamer.createMessageData(static_cast<hsize_t>(1), 1);

  std::string rawbuf;
  eventData[0]->getBufferPointer(rawbuf, messageID);

  auto receivedEventData =
      EventData(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
  EXPECT_EQ(770, receivedEventData.getNumberOfEvents());
  EXPECT_EQ(300, receivedEventData.getNumberOfFrames());
  EXPECT_EQ(1, receivedEventData.getFrameNumber());
}

TEST(NexusPublisherTest, test_create_message_data_3_message_per_frame) {
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";
  uint64_t messageID = 0;

  auto publisher = std::make_shared<MockEventPublisher>();

  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusPublisher streamer(publisher, broker, topic,
                          testDataPath + "SANS_test_reduced.hdf5", true, false);
  auto eventData = streamer.createMessageData(static_cast<hsize_t>(1), 3);

  std::string rawbuf;
  eventData[0]->getBufferPointer(rawbuf, messageID);

  auto receivedEventData =
      EventData(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
  // First message should have ceil(770/3) events
  EXPECT_EQ(257, receivedEventData.getNumberOfEvents());
  EXPECT_EQ(300, receivedEventData.getNumberOfFrames());
  EXPECT_EQ(1, receivedEventData.getFrameNumber());
  // and should not be the last message in the frame or in the run
  EXPECT_FALSE(receivedEventData.getEndFrame());
  EXPECT_FALSE(receivedEventData.getEndRun());

  eventData[2]->getBufferPointer(rawbuf, messageID);
  receivedEventData =
      EventData(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
  // Last message should have remaining 256 events
  EXPECT_EQ(256, receivedEventData.getNumberOfEvents());
  // and should be the last message in the frame but not in the run
  EXPECT_TRUE(receivedEventData.getEndFrame());
  EXPECT_FALSE(receivedEventData.getEndRun());
}

MATCHER_P(CheckMessageID, messageID, "") {
  auto buf = reinterpret_cast<const uint8_t *>(arg);
  auto messageData = GetEventMessage(buf);
  return (messageID == messageData->id());
}

TEST(NexusPublisherTest, test_stream_data) {
  using ::testing::Sequence;
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();

  const int numberOfFrames = 300;
  const int maxEventsPerFramePart = 1000000;

  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(1);

  // test that messages have sequential id numbers
  Sequence s1;
  for (uint64_t messageID = 0; messageID < numberOfFrames;
       messageID++) {
    EXPECT_CALL(*publisher.get(), sendMessage(CheckMessageID(messageID), _))
        .InSequence(s1);
  }

  NexusPublisher streamer(publisher, broker, topic,
                          testDataPath + "SANS_test_reduced.hdf5", false,
                          false);
  EXPECT_NO_THROW(streamer.streamData(maxEventsPerFramePart));
}

TEST(NexusPublisherTest, test_stream_data_multiple_messages_per_frame) {

  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();

  const int maxEventsPerFramePart = 200;

  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(1);
  EXPECT_CALL(*publisher.get(), sendMessage(_, _))
      .Times(1291);

  NexusPublisher streamer(publisher, broker, topic,
                          testDataPath + "SANS_test_reduced.hdf5", false,
                          false);
  EXPECT_NO_THROW(streamer.streamData(maxEventsPerFramePart));
}
