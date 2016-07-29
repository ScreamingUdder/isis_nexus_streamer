#include "EventData.h"
#include <gtest/gtest.h>

class EventDataTest : public ::testing::Test {};

TEST(EventDataTest, get_tof_and_detid) {
  auto events = EventData();
  EXPECT_EQ(0, events.getTof().size());
  EXPECT_EQ(0, events.getDetId().size());
  EXPECT_EQ(0, events.getNumberOfFrames());
  EXPECT_EQ(0, events.getFrameNumber());
}

TEST(EventDataTest, get_buffer_pointer) {
  auto events = EventData();

  std::vector<uint32_t> detIds = {1, 2, 3, 4};
  std::vector<uint64_t> tofs = {4, 3, 2, 1};
  uint32_t frameNumber = 2;
  uint32_t numberOfFrames = 10;

  EXPECT_NO_THROW(events.setDetId(detIds));
  EXPECT_NO_THROW(events.setTof(tofs));
  EXPECT_NO_THROW(events.setNumberOfFrames(numberOfFrames));
  EXPECT_NO_THROW(events.setFrameNumber(frameNumber));

  std::string rawbuf;
  EXPECT_NO_THROW(events.getBufferPointer(rawbuf));

  auto receivedEventData =
      EventData(rawbuf.c_str(), rawbuf.size());
  EXPECT_EQ(4, receivedEventData.getNumberOfEvents());
  EXPECT_EQ(detIds, receivedEventData.getDetId());
  EXPECT_EQ(tofs, receivedEventData.getTof());
  EXPECT_EQ(numberOfFrames, receivedEventData.getNumberOfFrames());
  EXPECT_EQ(frameNumber, receivedEventData.getFrameNumber());
}

TEST(EventDataTest, get_buffer_size) {
  auto events = EventData();

  std::vector<uint32_t> detIds = {1, 2, 3, 4};
  std::vector<uint64_t> tofs = {4, 3, 2, 1};

  events.setDetId(detIds);
  events.setTof(tofs);

  std::string rawbuf;
  EXPECT_NO_THROW(events.getBufferPointer(rawbuf));
  if (events.getUsingSnappy()) {
    EXPECT_EQ(74, events.getBufferSize());
  } else {
    EXPECT_EQ(96, events.getBufferSize());
  }
}
