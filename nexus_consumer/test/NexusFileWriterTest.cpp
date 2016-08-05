#include <gtest/gtest.h>

#include "../../nexus_file_reader/include/NexusFileReader.h"
#include "EventData.h"
#include "NexusFileWriter.h"

class NexusFileWriterTest : public ::testing::Test {
public:
  virtual void TearDown() {
    // clean up, delete temporary test file
    extern std::string testDataPath;
    const std::string testfileFullPath = testDataPath + m_testFilename;
    if (file_exists(testfileFullPath)) {
      std::remove(testfileFullPath.c_str());
    }
  }

  bool file_exists(const std::string &name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }
  }

  std::shared_ptr<EventData> createEventData(uint32_t frameNumber,
                                             uint32_t numberOfFrames,
                                             uint64_t totalCounts) {
    auto exampleEventData = std::make_shared<EventData>();

    std::vector<uint32_t> detIds = {1, 2, 3, 4};
    std::vector<uint64_t> tofs = {4, 3, 2, 1};

    exampleEventData->setDetId(detIds);
    exampleEventData->setTof(tofs);
    exampleEventData->setNumberOfFrames(numberOfFrames);
    exampleEventData->setFrameNumber(frameNumber);
    exampleEventData->setTotalCounts(totalCounts);

    return exampleEventData;
  };

  const std::string m_testFilename = "created_test_file.hdf5";
};

TEST_F(NexusFileWriterTest, creates_output_file) {
  extern std::string testDataPath;

  EXPECT_NO_THROW(NexusFileWriter(testDataPath + m_testFilename));
  EXPECT_TRUE(file_exists(testDataPath + m_testFilename));
}

TEST_F(NexusFileWriterTest, write_frame_to_file) {
  extern std::string testDataPath;
  auto fileWriter = std::unique_ptr<NexusFileWriter>(
      new NexusFileWriter(testDataPath + m_testFilename));

  auto testEventData = createEventData(0, 1, 4);
  EXPECT_NO_THROW(fileWriter->writeData(testEventData));

  // ensure the writer is not busy with the file when we try to read from it
  fileWriter.reset();

  auto fileReader = NexusFileReader(testDataPath + m_testFilename);
  EXPECT_EQ(4, fileReader.getTotalEventCount());
  EXPECT_EQ(1, fileReader.getNumberOfFrames());
  std::vector<uint32_t> detIds;
  std::vector<uint64_t> tofs;
  EXPECT_TRUE(fileReader.getEventDetIds(detIds, 0));
  EXPECT_TRUE(fileReader.getEventTofs(tofs, 0));
  EXPECT_FLOAT_EQ(1, detIds[0]);
  EXPECT_FLOAT_EQ(4, tofs[0]);
}

TEST_F(NexusFileWriterTest, write_multiple_frames_to_file) {
  extern std::string testDataPath;
  auto fileWriter = std::unique_ptr<NexusFileWriter>(
      new NexusFileWriter(testDataPath + m_testFilename));

  auto testEventData1 = createEventData(0, 2, 8);
  auto testEventData2 = createEventData(1, 2, 8);
  EXPECT_NO_THROW(fileWriter->writeData(testEventData1));
  EXPECT_NO_THROW(fileWriter->writeData(testEventData2));

  // ensure the writer is not busy with the file when we try to read from it
  fileWriter.reset();

  auto fileReader = NexusFileReader(testDataPath + m_testFilename);
  EXPECT_EQ(8, fileReader.getTotalEventCount());
  EXPECT_EQ(2, fileReader.getNumberOfFrames());
  std::vector<uint32_t> detIds;
  std::vector<uint64_t> tofs;
  EXPECT_TRUE(fileReader.getEventDetIds(detIds, 0));
  EXPECT_TRUE(fileReader.getEventTofs(tofs, 0));
  EXPECT_FLOAT_EQ(1, detIds[0]);
  EXPECT_FLOAT_EQ(4, tofs[0]);

  EXPECT_TRUE(fileReader.getEventDetIds(detIds, 1));
  EXPECT_TRUE(fileReader.getEventTofs(tofs, 1));
  EXPECT_FLOAT_EQ(1, detIds[0]);
  EXPECT_FLOAT_EQ(4, tofs[0]);
}

TEST_F(NexusFileWriterTest, write_multiple_frames_and_messages_to_file) {
  extern std::string testDataPath;
  auto fileWriter = std::unique_ptr<NexusFileWriter>(
      new NexusFileWriter(testDataPath + m_testFilename));

  auto testEventData1 = createEventData(0, 2, 16);
  auto testEventData2 = createEventData(0, 2, 16);
  auto testEventData3 = createEventData(1, 2, 16);
  auto testEventData4 = createEventData(1, 2, 16);
  EXPECT_NO_THROW(fileWriter->writeData(testEventData1));
  EXPECT_NO_THROW(fileWriter->writeData(testEventData2));
  EXPECT_NO_THROW(fileWriter->writeData(testEventData3));
  EXPECT_NO_THROW(fileWriter->writeData(testEventData4));

  // ensure the writer is not busy with the file when we try to read from it
  fileWriter.reset();

  auto fileReader = NexusFileReader(testDataPath + m_testFilename);
  EXPECT_EQ(16, fileReader.getTotalEventCount());
  EXPECT_EQ(2, fileReader.getNumberOfFrames());
  std::vector<uint32_t> detIds;
  std::vector<uint64_t> tofs;
  EXPECT_TRUE(fileReader.getEventDetIds(detIds, 0));
  EXPECT_TRUE(fileReader.getEventTofs(tofs, 0));
  EXPECT_FLOAT_EQ(1, detIds[0]);
  EXPECT_FLOAT_EQ(4, tofs[0]);

  EXPECT_TRUE(fileReader.getEventDetIds(detIds, 1));
  EXPECT_TRUE(fileReader.getEventTofs(tofs, 1));
  EXPECT_FLOAT_EQ(1, detIds[0]);
  EXPECT_FLOAT_EQ(4, tofs[0]);
}
