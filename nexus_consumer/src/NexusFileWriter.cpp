#include <iostream>

#include "NexusFileWriter.h"

using namespace H5;

NexusFileWriter::NexusFileWriter(const std::string &filename)
    : m_file(new H5File(filename, H5F_ACC_TRUNC)) {}

void NexusFileWriter::writeData(std::shared_ptr<EventData> eventData,
                                const bool finalData) {
  if (m_eventsSoFar == 0) {
    // This is the first lot of data so the datasets must be created here
    auto group = m_file->createGroup("/raw_data_1");
    group.createGroup("detector_1_events");
    writeScalarDataset(static_cast<uint64_t>(m_eventsSoFar),
                       "/raw_data_1/detector_1_events/event_index",
                       PredType::NATIVE_UINT64);
    writeTofs(eventData->getTof());
    writeDetIds(eventData->getDetId());
    writeScalarDataset(eventData->getTotalCounts(),
                       "/raw_data_1/detector_1_events/total_counts",
                       PredType::NATIVE_UINT64);
  } else {
    // TODO append data to existing datasets
    // TODO write current m_eventsSoFar as the next element of event_index
  }

  m_eventsSoFar += eventData->getNumberOfEvents();

  if (finalData) {
    writeScalarDataset(static_cast<int32_t>(eventData->getNumberOfFrames()),
                       "/raw_data_1/good_frames", PredType::NATIVE_INT32);
  }
}

void NexusFileWriter::writeDetIds(const std::vector<uint32_t> &detids) {
  std::vector<uint32_t> detids_u32(detids.begin(), detids.end());
  write1DDataset(detids_u32, "/raw_data_1/detector_1_events/event_id",
                 PredType::NATIVE_UINT32);
}

void NexusFileWriter::writeTofs(const std::vector<uint64_t> &tofs) {
  std::vector<float> tofs_float(tofs.begin(), tofs.end());
  std::transform(tofs_float.begin(), tofs_float.end(), tofs_float.begin(),
                 std::bind1st(std::multiplies<float>(), 0.001));
  write1DDataset(tofs_float, "/raw_data_1/detector_1_events/event_time_offset",
                 PredType::NATIVE_FLOAT);
}

void NexusFileWriter::writeScalarDataset(const int64_t value,
                                         const std::string &datasetName,
                                         PredType datatype) {
  hsize_t dim1[] = {1};
  DataSpace scalarValueDataspace(m_rank, dim1);
  auto dataset =
      m_file->createDataSet(datasetName, datatype, scalarValueDataspace);
  dataset.write(&value, datatype);
}

template <typename T>
void NexusFileWriter::write1DDataset(const std::vector<T> &values,
                                     const std::string &datasetName,
                                     PredType datatype) {
  hsize_t dim1[] = {values.size()};
  DataSpace scalarValueDataspace(m_rank, dim1);
  auto dataset =
      m_file->createDataSet(datasetName, datatype, scalarValueDataspace);
  dataset.write(values.data(), datatype);
}
