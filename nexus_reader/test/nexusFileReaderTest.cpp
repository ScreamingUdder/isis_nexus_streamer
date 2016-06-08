#include <gtest/gtest.h>

#include "../inc/nexusFileReader.h"

class NexusFileReaderTest : public ::testing::Test {};

TEST(NexusFileReaderTest, nexus_file_open_not_exist) {
  H5::Exception::dontPrint();
  extern std::string testDataPath;
  EXPECT_THROW(NexusFileReader(testDataPath+"not_exist_file.nxs"), H5::FileIException);
}

TEST(NexusFileReaderTest, nexus_file_open_exists) {
  extern std::string testDataPath;
  EXPECT_NO_THROW(NexusFileReader(testDataPath+"SANS_test.nxs"));
}

TEST(NexusFileReaderTest, nexus_read_file_size) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"SANS_test.nxs");
  EXPECT_EQ(104602171, fileReader.getFileSize());
}

TEST(NexusFileReaderTest, nexus_read_number_events) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"SANS_test.nxs");
  EXPECT_EQ(14258850, fileReader.getTotalEventCount());
}

TEST(NexusFileReaderTest, nexus_read_number_frames) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"SANS_test.nxs");
  EXPECT_EQ(18131, fileReader.getNumberOfFrames());
}

TEST(NexusFileReaderTest, get_detIds_first_frame) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"SANS_test.nxs");
  std::vector<uint32_t> detIds;
  EXPECT_TRUE(fileReader.getEventDetIds(detIds, 0));
  EXPECT_FLOAT_EQ(99406, detIds[0]);
  EXPECT_FLOAT_EQ(87829, detIds[150]);
}

/*
TEST(NexusFileReaderTest, get_event_tofs) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"SANS_test.nxs");
  std::vector<uint64_t> eventTofs;
  fileReader.getEventTofs(std::vector &eventTofs);
  EXPECT_FLOAT_EQ(123.0, eventTofs[0]);
  EXPECT_FLOAT_EQ(234.0, eventTofs[150]);
}
 */
