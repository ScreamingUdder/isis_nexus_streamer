#include "nexusFileReader.h"
#include <iostream>

using namespace H5;

NexusFileReader::NexusFileReader(const std::string &filename)
    : m_maxEventSubsetSize(1024) {
  m_file = std::make_shared<H5File>(filename, H5F_ACC_RDONLY);
}

NexusFileReader::NexusFileReader(const std::string &filename,
                                 size_t eventSubsetSize)
    : m_maxEventSubsetSize(eventSubsetSize) {
  m_file = std::make_shared<H5File>(filename, H5F_ACC_RDONLY);
}

hsize_t NexusFileReader::getFileSize() { return m_file->getFileSize(); }

uint64_t NexusFileReader::getTotalEventCount() {
  DataSet dataset = m_file->openDataSet(
      "/raw_data_1/detector_1_events/total_counts"); // total_counts
  uint64_t *totalCount = new uint64_t[1];
  dataset.read(totalCount, PredType::NATIVE_UINT64);
  return *totalCount;
}
