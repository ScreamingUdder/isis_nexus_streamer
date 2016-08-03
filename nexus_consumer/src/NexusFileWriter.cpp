#include <iostream>

#include "NexusFileWriter.h"

using namespace H5;

NexusFileWriter::NexusFileWriter(const std::string &filename)
    : m_file(new H5File(filename, H5F_ACC_TRUNC)) {}

void NexusFileWriter::writeData(std::shared_ptr<EventData> eventData) {
  if (m_eventsSoFar == 0) {
    // This is the first lot of data so the datasets must be created here
    auto group = m_file->createGroup("/raw_data_1");
    group.createGroup("detector_1_events");
    std::vector<uint64_t> eventIndices = {static_cast<uint64_t>(m_eventsSoFar)};
    create1DDataset(eventIndices, "/raw_data_1/detector_1_events/event_index",
                    PredType::NATIVE_UINT64, eventData->getNumberOfFrames());
    uint64_t totalCounts = eventData->getTotalCounts();
    writeTofs(eventData->getTof(), totalCounts);
    writeDetIds(eventData->getDetId(), totalCounts);
    createScalarDataset(totalCounts,
                        "/raw_data_1/detector_1_events/total_counts",
                        PredType::NATIVE_UINT64);
    createScalarDataset(static_cast<int32_t>(eventData->getNumberOfFrames()),
                        "/raw_data_1/good_frames", PredType::NATIVE_INT32);
  } else {
    // append data to existing datasets
    writeTofs(eventData->getTof(), 0);
    writeDetIds(eventData->getDetId(), 0);
    writeEventIndex(eventData->getFrameNumber());
  }

  m_eventsSoFar += eventData->getNumberOfEvents();
}

void NexusFileWriter::writeEventIndex(uint32_t currentFrameNumber) {
  // if current event data is from a new frame then we record the event index
  // for the start of this frame
  if (currentFrameNumber != m_previousFrameNumber) {
    std::vector<uint64_t> eventIndices = {static_cast<uint64_t>(m_eventsSoFar)};
    append1DDataset(eventIndices, "/raw_data_1/detector_1_events/event_index",
                    currentFrameNumber, PredType::NATIVE_UINT64);
    m_previousFrameNumber = currentFrameNumber;
  }
}

void NexusFileWriter::writeDetIds(const std::vector<uint32_t> &detids,
                                  const uint64_t datasize) {
  const std::string datasetName = "/raw_data_1/detector_1_events/event_id";
  std::vector<uint32_t> detids_u32(detids.begin(), detids.end());
  if (datasize != 0) {
    detids_u32.resize(datasize);
    create1DDataset(detids_u32, datasetName, PredType::NATIVE_UINT32, datasize);
  } else {
    append1DDataset(detids, datasetName, static_cast<hsize_t>(m_eventsSoFar),
                    PredType::NATIVE_UINT32);
  }
}

void NexusFileWriter::writeTofs(const std::vector<uint64_t> &tofs,
                                const uint64_t datasize) {
  const std::string datasetName =
      "/raw_data_1/detector_1_events/event_time_offset";
  std::vector<float> tofs_float(tofs.begin(), tofs.end());
  std::transform(tofs_float.begin(), tofs_float.end(), tofs_float.begin(),
                 std::bind1st(std::multiplies<float>(), 0.001));
  if (datasize != 0) {
    tofs_float.resize(datasize);
    create1DDataset(tofs_float, datasetName, PredType::NATIVE_FLOAT, datasize);
  } else {
    append1DDataset(tofs_float, datasetName,
                    static_cast<hsize_t>(m_eventsSoFar),
                    PredType::NATIVE_FLOAT);
  }
}

void NexusFileWriter::createScalarDataset(const int64_t value,
                                          const std::string &datasetName,
                                          PredType datatype) {
  hsize_t dim1[] = {1};
  DataSpace scalarValueDataspace(m_rank, dim1);
  auto dataset =
      m_file->createDataSet(datasetName, datatype, scalarValueDataspace);
  dataset.write(&value, datatype);
}

template <typename T>
void NexusFileWriter::create1DDataset(const std::vector<T> &values,
                                      const std::string &datasetName,
                                      PredType datatype,
                                      const uint64_t datasize) {
  hsize_t dim1[] = {datasize};
  DataSpace scalarValueDataspace(m_rank, dim1);
  auto dataset =
      m_file->createDataSet(datasetName, datatype, scalarValueDataspace);
  dataset.write(values.data(), datatype);
}

template <typename T>
void NexusFileWriter::append1DDataset(const std::vector<T> &values,
                                      const std::string &datasetName,
                                      hsize_t startIndex, PredType datatype) {
  // Get the dataspace where the data will be stored and select the hyperslab
  // which the values will replace
  auto dataset = m_file->openDataSet(datasetName);
  auto fspace = dataset.getSpace();
  hsize_t count[1], start[1], stride[1], block[1];
  start[0] = startIndex;
  block[0] = 1;
  stride[0] = 1;
  count[0] = values.size();
  fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

  // Define the memoryspace of the values which will be put into the dataset
  hsize_t dim1[] = {values.size()};
  DataSpace mspace(m_rank, dim1);

  dataset.write(values.data(), datatype, mspace, fspace);
}