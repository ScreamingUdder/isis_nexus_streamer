#include <iostream>
#include "nexusFileReader.h"

using namespace H5;

NexusFileReader::NexusFileReader(const std::string &filename) {
  m_file = std::make_shared<H5File>(filename, H5F_ACC_RDONLY);
}

hsize_t NexusFileReader::getFileSize() {
  return m_file->getFileSize();
}

uint64_t NexusFileReader::getTotalEventCount() {
  DataSet dataset = m_file->openDataSet("/raw_data_1/detector_1_events/total_counts"); //total_counts
  uint64_t *totalCount = new uint64_t[1];
  dataset.read(totalCount, PredType::NATIVE_UINT64);
  return *totalCount;
}
