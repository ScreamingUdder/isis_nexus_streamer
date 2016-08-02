#include <gtest/gtest.h>

#include "NexusFileWriter.h"

class NexusFileWriterTest : public ::testing::Test {
public:

  virtual void TearDown() {
    // clean up, delete temporary test file
    extern std::string testDataPath;
    const std::string testfileFullPath = testDataPath + m_testFilename;
    if (file_exists(testfileFullPath)){
      std::remove(testfileFullPath.c_str());
    }
  }

  bool file_exists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }
  }

  const std::string m_testFilename = "created_test_file.hdf5";
};

TEST_F(NexusFileWriterTest, creates_output_file) {
extern std::string testDataPath;

EXPECT_NO_THROW(NexusFileWriter(testDataPath + m_testFilename));
EXPECT_TRUE(file_exists(testDataPath + m_testFilename));
}
