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
    writeScalarDataset(m_eventsSoFar,
                       "/raw_data_1/detector_1_events/event_index");
    write1DDataset(eventData->getTof(),
                   "/raw_data_1/detector_1_events/event_time_offset",
                   PredType::NATIVE_INT64);
    auto ids = eventData->getDetId();
    write1DDataset(ids, "/raw_data_1/detector_1_events/event_id",
                   PredType::NATIVE_INT32);
  } else {
    // TODO append data to existing datasets
    // TODO write current m_eventsSoFar as the next element of event_index
  }

  m_eventsSoFar += eventData->getNumberOfEvents();

  if (finalData) {
    int64_t good_frames = eventData->getNumberOfFrames();
    writeScalarDataset(good_frames, "/raw_data_1/good_frames");
    writeScalarDataset(m_eventsSoFar,
                       "/raw_data_1/detector_1_events/total_counts");
  }
}

void NexusFileWriter::writeScalarDataset(const int64_t value,
                                         const std::string &datasetName) {
  hsize_t dim1[] = {1};
  DataSpace scalarValueDataspace(m_rank, dim1);
  auto dataset = m_file->createDataSet(datasetName, PredType::NATIVE_INT64,
                                       scalarValueDataspace);
  dataset.write(&value, PredType::NATIVE_INT64);
}

template <typename T>
void NexusFileWriter::write1DDataset(const std::vector<T> &values,
                                     const std::string &datasetName,
                                     PredType datatype) {
  hsize_t dim1[] = {values.size()};
  DataSpace scalarValueDataspace(m_rank, dim1);
  auto dataset = m_file->createDataSet(datasetName, PredType::NATIVE_INT64,
                                       scalarValueDataspace);
  dataset.write(values.data(), datatype);
}
