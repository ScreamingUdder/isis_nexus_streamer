#include "../inc/eventdata.h"
#include <gtest/gtest.h>

class EventDataTest : public ::testing::Test {};

TEST(EventDataTest, get_tof_and_detid) {
  auto events = EventData();
  EXPECT_EQ(0, events.getTof().size());
  EXPECT_EQ(0, events.getDetid().size());
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
