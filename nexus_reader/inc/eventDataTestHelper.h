#ifndef ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H
#define ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H

#include "eventDataFlatBuffer_generated.h"

class EventDataTestHelper {
public:
  EventDataTestHelper(const uint8_t *);

  std::vector<int32_t> getDetId() { return m_detId; };
  std::vector<float> getTof() { return m_tof; };
  int32_t getCount() { return m_count; };

private:
  std::vector<int32_t> m_detId;
  std::vector<float> m_tof;
  int32_t m_count;
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H
