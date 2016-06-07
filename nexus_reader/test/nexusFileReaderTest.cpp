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
  EXPECT_NO_THROW(NexusFileReader(testDataPath+"WISH_test.nxs"));
}

TEST(NexusFileReaderTest, nexus_read_file_size) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"WISH_test.nxs");
  EXPECT_EQ(fileReader.getFileSize(), 1489857698);
}

TEST(NexusFileReaderTest, nexus_read_number_events) {
  extern std::string testDataPath;
  auto fileReader = NexusFileReader(testDataPath+"WISH_test.nxs");
  EXPECT_EQ(fileReader.getTotalCounts(), 120529245);
}
