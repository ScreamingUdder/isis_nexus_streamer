#include "../include/EventData.h"

EventData::EventData() : m_bufferSize(0) {};

void EventData::setDetId(std::vector<uint32_t> detIds) {
  m_detId = detIds;
}

void EventData::setTof(std::vector<uint64_t> tofs) {
  m_tof = tofs;
}

flatbuffers::unique_ptr_t EventData::getBufferPointer(std::string &buffer) {
  flatbuffers::FlatBufferBuilder builder;

  auto detIdsVector = builder.CreateVector(m_detId);
  auto tofsVector = builder.CreateVector(m_tof);

  auto messageFlatbuf = CreateFlatbufEventData(
      builder, static_cast<int32_t>(m_detId.size()), detIdsVector, tofsVector);
  builder.Finish(messageFlatbuf);

  auto bufferpointer =
      reinterpret_cast<const char *>(builder.GetBufferPointer());
  buffer.assign(bufferpointer, bufferpointer + builder.GetSize());

  m_bufferSize = builder.GetSize();

  return builder.ReleaseBufferPointer();
}

std::vector<uint32_t> EventData::getDetId() { return m_detId; }

std::vector<uint64_t> EventData::getTof() { return m_tof; }
