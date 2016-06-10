#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "MockEventPublisher.h"
#include "NexusStreamer.h"

using ::testing::AtLeast;

class NexusStreamerTest : public ::testing::Test {};

TEST(NexusStreamerTest, test_setup_streamer) {
  auto publisher = std::make_shared<MockEventPublisher>();
  EXPECT_CALL(*publisher.get(), setUp("something1", "something2"))
      .Times(AtLeast(1));

  NexusStreamer streamer(publisher);
}
