#ifndef ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H
#define ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H

#include "eventDataFlatBuffer_generated.h"

class EventDataTestHelper {
public:
  EventDataTestHelper(const uint8_t *);

  std::vector<uint32_t> getDetId() { return m_detId; };
  std::vector<uint64_t> getTof() { return m_tof; };
  int32_t getCount() { return m_count; };
  int32_t getNumberOfFrames() { return m_numberOfFrames; };
  int32_t getFrameNumber() { return m_frameNumber; };

private:
  std::vector<uint32_t> m_detId;
  std::vector<uint64_t> m_tof;
  int32_t m_count;
  int32_t m_numberOfFrames;
  int32_t m_frameNumber;
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATATESTHELPER_H
