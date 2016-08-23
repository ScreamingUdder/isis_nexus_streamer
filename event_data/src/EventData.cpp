#include "EventData.h"
#include <iostream>

EventData::EventData()
    : m_bufferSize(0), m_frameNumber(0), m_numberOfFrames(0), m_totalCounts(0) {
}

EventData::EventData(const uint8_t *buf) { decodeMessage(buf); }

uint64_t EventData::getMessageID(const std::string &rawbuf) {
  auto buf = reinterpret_cast<const uint8_t *>(rawbuf.c_str());
  auto messageData = GetEventMessage(buf);
  return messageData->id();
}

void EventData::decodeMessage(const uint8_t *buf) {
  auto messageData = GetEventMessage(buf);
  if (messageData->message_type() == MessageTypes_FlatbufEventData) {
    auto eventData =
        static_cast<const FlatbufEventData *>(messageData->message());
    auto detIdFBVector = eventData->detId();
    auto tofFBVector = eventData->tof();

    m_detId.resize(static_cast<size_t>(eventData->count()));
    m_tof.resize(static_cast<size_t>(eventData->count()));
    std::copy(detIdFBVector->begin(), detIdFBVector->end(), m_detId.begin());
    std::copy(tofFBVector->begin(), tofFBVector->end(), m_tof.begin());

    setNumberOfFrames(eventData->totalFrames());
    setFrameNumber(eventData->frameNumber());
    setTotalCounts(eventData->totalCounts());
  } else {
    std::cout << "Unrecognised mesage type" << std::endl;
  }
}

flatbuffers::unique_ptr_t EventData::getBufferPointer(std::string &buffer, uint64_t messageID) {
  flatbuffers::FlatBufferBuilder builder;

  auto detIdsVector = builder.CreateVector(m_detId);
  auto tofsVector = builder.CreateVector(m_tof);

  auto messageFlatbuf = CreateFlatbufEventData(
      builder, static_cast<int32_t>(m_detId.size()), detIdsVector, tofsVector,
      m_frameNumber, m_numberOfFrames, m_totalCounts);

  auto messageOffset = CreateEventMessage(
      builder, MessageTypes_FlatbufEventData, messageFlatbuf.Union(), messageID);

  builder.Finish(messageOffset);

  auto bufferpointer =
      reinterpret_cast<const char *>(builder.GetBufferPointer());
  buffer.assign(bufferpointer, bufferpointer + builder.GetSize());

  m_bufferSize = builder.GetSize();

  return builder.ReleaseBufferPointer();
}
