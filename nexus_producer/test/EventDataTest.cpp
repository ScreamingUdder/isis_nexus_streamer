#include "EventData.h"
#include "EventDataTestHelper.h"
#include <gtest/gtest.h>

class EventDataTest : public ::testing::Test {};

TEST(EventDataTest, get_tof_and_detid) {
  auto events = EventData();
  EXPECT_EQ(0, events.getTof().size());
  EXPECT_EQ(0, events.getDetId().size());
}

TEST(EventDataTest, get_buffer_pointer) {
  auto events = EventData();

  std::vector<uint32_t> detIds = {1, 2, 3, 4};
  std::vector<uint64_t> tofs = {4, 3, 2, 1};

  events.setDetId(detIds);
  events.setTof(tofs);

  std::string rawbuf;
  events.getBufferPointer(rawbuf);

  auto testHelper =
      EventDataTestHelper(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
  EXPECT_EQ(4, testHelper.getCount());
  EXPECT_EQ(detIds, testHelper.getDetId());
  EXPECT_EQ(tofs, testHelper.getTof());
}
