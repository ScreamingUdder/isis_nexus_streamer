#include "nexusFileReader.h"
#include <iostream>

using namespace H5;

NexusFileReader::NexusFileReader(const std::string &filename) {
  m_file = std::make_shared<H5File>(filename, H5F_ACC_RDONLY);
  DataSet dataset = m_file->openDataSet(
      "/raw_data_1/good_frames");
  size_t *numOfFrames = new size_t[1];
  dataset.read(numOfFrames, PredType::NATIVE_UINT64);
  m_numberOfFrames = *numOfFrames;
}

hsize_t NexusFileReader::getFileSize() { return m_file->getFileSize(); }

uint64_t NexusFileReader::getTotalEventCount() {
  DataSet dataset = m_file->openDataSet(
      "/raw_data_1/detector_1_events/total_counts"); // total_counts
  uint64_t *totalCount = new uint64_t[1];
  dataset.read(totalCount, PredType::NATIVE_UINT64);
  return *totalCount;
}

bool NexusFileReader::getEventDetIds(std::vector<uint32_t> &detIds) {
  m_frameNumber++;
  if (m_frameNumber > m_numberOfFrames)
  DataSet dataset = m_file->openDataSet(
      "/raw_data_1/detector_1_events/event_index");
}
