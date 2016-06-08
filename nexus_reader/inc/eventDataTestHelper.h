#ifndef ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H
#define ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H

#include "eventDataFlatBuffer_generated.h"

class EventDataTestHelper {
public:
  EventDataTestHelper(const uint8_t *);

  std::vector<uint32_t> getDetId() { return m_detId; };
  std::vector<uint64_t> getTof() { return m_tof; };
  int32_t getCount() { return m_count; };

private:
  std::vector<uint32_t> m_detId;
  std::vector<uint64_t> m_tof;
  int32_t m_count;
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H
