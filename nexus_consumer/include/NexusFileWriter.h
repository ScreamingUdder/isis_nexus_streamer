#ifndef ISIS_NEXUS_STREAMER_NEXUSFILEWRITER_H_H
#define ISIS_NEXUS_STREAMER_NEXUSFILEWRITER_H_H

#include <H5Cpp.h>
#include <memory>
#include <vector>

#include "EventData.h"

typedef std::unique_ptr<H5::H5File> H5FilePtr;

class NexusFileWriter {
public:
  NexusFileWriter(const std::string &filename);

  void writeData(std::shared_ptr<EventData> eventData);

private:
  void createScalarDataset(const int64_t value, const std::string &datasetName,
                           H5::PredType datatype);
  template <typename T>
  void create1DDataset(const std::vector<T> &values,
                       const std::string &datasetName, H5::PredType datatype,
                       const uint64_t datasize);
  void writeTofs(const std::vector<uint64_t> &tofs, const uint64_t datasize);
  void writeDetIds(const std::vector<uint32_t> &detids,
                   const uint64_t datasize);
  template <typename T>
  void append1DDataset(const std::vector<T> &values,
                       const std::string &datasetName, hsize_t startIndex,
                       H5::PredType datatype);
  void writeEventIndex(uint32_t currentFrameNumber);

  H5FilePtr m_file = nullptr;
  int64_t m_eventsSoFar = 0;
  uint32_t m_previousFrameNumber = 0;
  const int m_rank = 1;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSFILEWRITER_H_H
