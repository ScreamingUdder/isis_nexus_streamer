#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "EventDataTestHelper.h"
#include "MockEventPublisher.h"
#include "NexusStreamer.h"

using ::testing::AtLeast;
using ::testing::_;

class NexusStreamerTest : public ::testing::Test {};

TEST(NexusStreamerTest, test_create_streamer) {
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();
  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusStreamer streamer(publisher, broker, topic,
                         testDataPath + "SANS_test.nxs");
}

TEST(NexusStreamerTest, test_create_message_data) {
  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();

  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusStreamer streamer(publisher, broker, topic,
                         testDataPath + "SANS_test.nxs");
  auto eventData = streamer.createMessageData(static_cast<hsize_t>(0));

  std::string rawbuf;
  eventData->getBufferPointer(rawbuf);

  auto testHelper =
      EventDataTestHelper(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
  EXPECT_EQ(794, testHelper.getCount());
}

TEST(NexusStreamerTest, test_stream_data) {

  extern std::string testDataPath;

  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto publisher = std::make_shared<MockEventPublisher>();

  EXPECT_CALL(*publisher.get(), setUp(broker, topic)).Times(1);
  EXPECT_CALL(*publisher.get(), sendMessage(_, _)).Times(AtLeast(1));

  NexusStreamer streamer(publisher, broker, topic,
                         testDataPath + "SANS_test.nxs");
  streamer.streamData(2);
}