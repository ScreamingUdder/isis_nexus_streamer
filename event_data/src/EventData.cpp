#include "../include/EventData.h"

EventData::EventData() : m_bufferSize(0), m_frameNumber(0), m_numberOfFrames(0) {};

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
