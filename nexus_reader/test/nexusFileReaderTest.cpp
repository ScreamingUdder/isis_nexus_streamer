#include <gtest/gtest.h>

#include "../inc/nexusFileReader.h"

class NexusFileReaderTest : public ::testing::Test {
protected:
  NexusFileReader fileReader = NexusFileReader();
};

TEST_F(NexusFileReaderTest, nexus_file_validity_not_exist) {
  //H5::Exception::dontPrint();
  extern std::string testDataPath;
  EXPECT_THROW(fileReader.openFile(testDataPath+"not_exist_file.nxs"), H5::FileIException);
}

TEST_F(NexusFileReaderTest, nexus_file_validity_valid_file) {
  extern std::string testDataPath;
  EXPECT_NO_THROW(fileReader.openFile(testDataPath+"WISH_test.nxs"));
}
