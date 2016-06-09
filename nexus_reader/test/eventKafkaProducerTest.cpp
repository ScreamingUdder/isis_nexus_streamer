#include <gtest/gtest.h>
//#include "../inc/eventKafkaProducer.h"

class EventKafkaProducerTest : public ::testing::Test {};

TEST(EventKafkaProducerTest, test_simple) {
  EXPECT_EQ(1, 2);
}
