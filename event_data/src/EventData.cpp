#include "EventData.h"

EventData::EventData() : m_bufferSize(0), m_frameNumber(0), m_numberOfFrames(0) {};

EventData::EventData(const uint8_t *buf) {
  auto eventData = GetFlatbufEventData(buf);
  auto detIdFBVector = eventData->detId();
  auto tofFBVector = eventData->tof();

  m_detId.resize(static_cast<size_t>(eventData->count()));
  m_tof.resize(static_cast<size_t>(eventData->count()));
  std::copy(detIdFBVector->begin(), detIdFBVector->end(), m_detId.begin());
  std::copy(tofFBVector->begin(), tofFBVector->end(), m_tof.begin());

  m_numberOfFrames = eventData->totalFrames();
  m_frameNumber = eventData->frameNumber();
}

flatbuffers::unique_ptr_t EventData::getBufferPointer(std::string &buffer) {
  flatbuffers::FlatBufferBuilder builder;

  auto detIdsVector = builder.CreateVector(m_detId);
  auto tofsVector = builder.CreateVector(m_tof);

  auto messageFlatbuf = CreateFlatbufEventData(
      builder, static_cast<int32_t>(m_detId.size()), detIdsVector, tofsVector,
      m_frameNumber, m_numberOfFrames);
  builder.Finish(messageFlatbuf);

  auto bufferpointer =
      reinterpret_cast<const char *>(builder.GetBufferPointer());
  buffer.assign(bufferpointer, bufferpointer + builder.GetSize());

  m_bufferSize = builder.GetSize();

  return builder.ReleaseBufferPointer();
}
