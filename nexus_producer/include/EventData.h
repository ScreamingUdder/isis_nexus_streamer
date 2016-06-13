#ifndef ISIS_NEXUS_STREAMER_EVENTDATA_H
#define ISIS_NEXUS_STREAMER_EVENTDATA_H

#include <cstdint>
#include <vector>

#include "eventDataFlatBuffer_generated.h"

class EventData {

public:
  EventData();

  std::vector<uint32_t> getDetId();
  std::vector<uint64_t> getTof();

  void setDetId(std::vector<uint32_t>);
  void setTof(std::vector<uint64_t>);

  flatbuffers::unique_ptr_t getBufferPointer(std::string &buffer);

  size_t getBufferSize() { return m_bufferSize; };

private:
  std::vector<uint32_t> m_detId = {};
  std::vector<uint64_t> m_tof = {};
  size_t m_bufferSize;
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATA_H
