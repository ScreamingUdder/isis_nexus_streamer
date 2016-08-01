#include "ProtoEventData.h"
#include <gtest/gtest.h>

class ProtoEventDataTest : public ::testing::Test {};

TEST(ProtoEventDataTest, get_tof_and_detid) {
auto events = ProtoEventData();
EXPECT_EQ(0, events.getTof().size());
EXPECT_EQ(0, events.getDetId().size());
EXPECT_EQ(0, events.getNumberOfFrames());
EXPECT_EQ(0, events.getFrameNumber());
}

TEST(ProtoEventDataTest, get_buffer_pointer) {
auto events = ProtoEventData();

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
    ProtoEventData(rawbuf);
EXPECT_EQ(4, receivedEventData.getNumberOfEvents());
EXPECT_EQ(detIds, receivedEventData.getDetId());
EXPECT_EQ(tofs, receivedEventData.getTof());
EXPECT_EQ(numberOfFrames, receivedEventData.getNumberOfFrames());
EXPECT_EQ(frameNumber, receivedEventData.getFrameNumber());
}

TEST(ProtoEventDataTest, get_buffer_size) {
auto events = ProtoEventData();

std::vector<uint32_t> detIds = {1, 2, 3, 4};
std::vector<uint64_t> tofs = {4, 3, 2, 1};

events.setDetId(detIds);
events.setTof(tofs);

std::string rawbuf;
EXPECT_NO_THROW(events.getBufferPointer(rawbuf));
EXPECT_EQ(14, events.getBufferSize());
}
