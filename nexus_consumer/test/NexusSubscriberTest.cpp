#include <gtest/gtest.h>

#include "EventData.h"
#include "MockEventSubscriber.h"
#include "NexusSubscriber.h"
#include "../../nexus_file_reader/include/NexusFileReader.h"

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
    uint64_t totalCounts = 50;

    exampleEventData->setDetId(detIds);
    exampleEventData->setTof(tofs);
    exampleEventData->setNumberOfFrames(numberOfFrames);
    exampleEventData->setFrameNumber(frameNumber);
    exampleEventData->setTotalCounts(totalCounts);

    return exampleEventData;
  }

  std::shared_ptr<EventData> createEventData(std::vector<uint32_t> detIds, uint32_t frameNumber) {
    auto exampleEventData = std::make_shared<EventData>();

    std::vector<uint64_t> tofs = {4, 3, 2, 1};
    uint32_t numberOfFrames = 3;
    uint64_t totalCounts = 12;

    exampleEventData->setDetId(detIds);
    exampleEventData->setTof(tofs);
    exampleEventData->setNumberOfFrames(numberOfFrames);
    exampleEventData->setFrameNumber(frameNumber);
    exampleEventData->setTotalCounts(totalCounts);

    return exampleEventData;
  }
};

TEST_F(NexusSubscriberTest, test_create_subscriber) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic, false, "");
}

TEST_F(NexusSubscriberTest, test_create_subscriber_which_writes_to_file) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  extern std::string testDataPath;
  const std::string testfileFullPath =
      testDataPath + "temp_unit_test_file.hdf5";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic, false, testfileFullPath);

  // clean up the file created
  std::remove(testfileFullPath.c_str());
}

TEST_F(NexusSubscriberTest, test_create_subscriber_quiet) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic, true, "");
}

TEST_F(NexusSubscriberTest, decode_received_message) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";
  uint64_t messageID = 0;

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  auto exampleEventData = createEventData();
  std::string rawbuf;
  EXPECT_NO_THROW(exampleEventData->getBufferPointer(rawbuf, messageID));

  // Decode the message
  auto receivedDataStats = std::make_shared<ReceivedDataStats>();
  auto receivedEvents = std::make_shared<EventData>();
  NexusSubscriber streamer(subscriber, broker, topic, true, "");
  EXPECT_NO_THROW(
      streamer.decodeMessage(receivedEvents, rawbuf, receivedDataStats));

  EXPECT_EQ(receivedEvents->getDetId(), exampleEventData->getDetId());
  EXPECT_EQ(receivedEvents->getTof(), exampleEventData->getTof());
  EXPECT_EQ(receivedEvents->getNumberOfFrames(),
            exampleEventData->getNumberOfFrames());
  EXPECT_EQ(receivedEvents->getFrameNumber(),
            exampleEventData->getFrameNumber());
  EXPECT_EQ(receivedEvents->getTotalCounts(),
            exampleEventData->getTotalCounts());
}

TEST_F(NexusSubscriberTest, test_listen_for_messages_one_received) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";
  uint64_t messageID = 0;

  auto exampleEventData = createEventData();
  // Make this the last frame so that listenForMessage will only be called once
  exampleEventData->setNumberOfFrames(9);
  exampleEventData->setFrameNumber(9);
  std::string rawbuf;
  EXPECT_NO_THROW(exampleEventData->getBufferPointer(rawbuf, messageID));

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic, true, "");

  // Should be called exactly once because the message contains the last frame
  EXPECT_CALL(*subscriber.get(), listenForMessage(_))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf), Return(true)));

  EXPECT_NO_THROW(streamer.listen());
}

TEST_F(NexusSubscriberTest, test_listen_for_messages_multiple_received) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";
  uint64_t messageID = 0;

  // Make this the second to last frame so that listenForMessage will be called
  // twice
  auto exampleEventData_first = createEventData();
  exampleEventData_first->setNumberOfFrames(9);
  exampleEventData_first->setFrameNumber(7);
  std::string rawbuf_first;
  EXPECT_NO_THROW(
      exampleEventData_first->getBufferPointer(rawbuf_first, messageID));

  // Last frame
  auto exampleEventData_second = createEventData();
  exampleEventData_second->setNumberOfFrames(9);
  exampleEventData_second->setFrameNumber(8);
  std::string rawbuf_second;
  EXPECT_NO_THROW(
      exampleEventData_second->getBufferPointer(rawbuf_second, messageID + 1));

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic, false, "");

  // Should be called exactly twice because there are messages containing the
  // last two frames
  EXPECT_CALL(*subscriber.get(), listenForMessage(_))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_first), Return(true)))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_second), Return(true)));

  EXPECT_NO_THROW(streamer.listen());
}

TEST_F(NexusSubscriberTest, test_listen_for_messages_received_out_of_order) {
  const std::string broker = "broker_name";
  const std::string topic = "topic_name";
  uint64_t messageID = 0;

  extern std::string testDataPath;
  const std::string testfileFullPath =
      testDataPath + "temp_unit_test_file.hdf5";

  // Make this the third to last frame so that listenForMessage will be called
  // three times
  std::vector<uint32_t> firstIds = {1, 1, 1, 1};
  auto exampleEventData_first = createEventData(firstIds, 0);
  std::string rawbuf_first;
  EXPECT_NO_THROW(
      exampleEventData_first->getBufferPointer(rawbuf_first, messageID));

  // Penultimate frame
  std::vector<uint32_t> secondIds = {2, 2, 2, 2};
  auto exampleEventData_second = createEventData(secondIds, 1);
  std::string rawbuf_second;
  EXPECT_NO_THROW(
      exampleEventData_second->getBufferPointer(rawbuf_second, messageID + 1));

  // Last frame
  std::vector<uint32_t> thirdIds = {3, 3, 3, 3};
  auto exampleEventData_third = createEventData(thirdIds, 2);
  std::string rawbuf_third;
  EXPECT_NO_THROW(
      exampleEventData_third->getBufferPointer(rawbuf_third, messageID + 2));

  auto subscriber = std::make_shared<MockEventSubscriber>();
  EXPECT_CALL(*subscriber.get(), setUp(broker, topic)).Times(AtLeast(1));

  NexusSubscriber streamer(subscriber, broker, topic, false, testfileFullPath);

  // Should be called exactly three times because there are messages containing the
  // last three frames
  EXPECT_CALL(*subscriber.get(), listenForMessage(_))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_first), Return(true)))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_third), Return(true)))
      .WillOnce(DoAll(SetArgReferee<0>(rawbuf_second), Return(true)));

  EXPECT_NO_THROW(streamer.listen());

  // Check data is in correct order in file
  auto fileReader = NexusFileReader(testfileFullPath);
  std::vector<uint32_t> firstIdsResult;
  fileReader.getEventDetIds(firstIdsResult, 0);
  std::vector<uint32_t> secondIdsResult;
  fileReader.getEventDetIds(secondIdsResult, 1);
  std::vector<uint32_t> thirdIdsResult;
  fileReader.getEventDetIds(thirdIdsResult, 2);

  EXPECT_THAT(firstIds, ::testing::ContainerEq(firstIdsResult));
  EXPECT_THAT(secondIds, ::testing::ContainerEq(secondIdsResult));
  EXPECT_THAT(thirdIds, ::testing::ContainerEq(thirdIdsResult));

  // clean up the file created
  std::remove(testfileFullPath.c_str());
}
