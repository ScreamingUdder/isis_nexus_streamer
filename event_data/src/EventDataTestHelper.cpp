#include "../include/EventDataTestHelper.h"

EventDataTestHelper::EventDataTestHelper(const uint8_t *buf) {
  auto eventData = GetFlatbufEventData(buf);
  auto detIdFBVector = eventData->detId();
  auto tofFBVector = eventData->tof();

  m_detId.resize(static_cast<size_t>(eventData->count()));
  m_tof.resize(static_cast<size_t>(eventData->count()));
  std::copy(detIdFBVector->begin(), detIdFBVector->end(), m_detId.begin());
  std::copy(tofFBVector->begin(), tofFBVector->end(), m_tof.begin());

  m_count = eventData->count();
  m_numberOfFrames = eventData->totalFrames();
  m_frameNumber = eventData->frameNumber();
}
