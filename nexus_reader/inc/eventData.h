#ifndef ISIS_NEXUS_STREAMER_EVENTDATA_H
#define ISIS_NEXUS_STREAMER_EVENTDATA_H

#include <cstdint>
#include <vector>

#include "eventDataFlatBuffer_generated.h"

class EventData {

public:

  EventData();

  std::vector<int32_t> getDetId();
  std::vector<float> getTof();

  void setDetId(std::vector<int32_t>);
  void setTof(std::vector<float>);

  flatbuffers::unique_ptr_t getBufferPointer(std::string &buffer);

private:
  std::vector<int32_t> m_detId = {};
  std::vector<float> m_tof = {};
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATA_H
