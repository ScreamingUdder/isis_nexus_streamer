#include "EventData.h"
#include <snappy.h>

EventData::EventData()
    : m_bufferSize(0), m_frameNumber(0), m_numberOfFrames(0){};

EventData::EventData(const char *buf, size_t buf_length) {
  decodeMessage(buf, buf_length);
}

void EventData::decodeMessage(const char *buf, size_t buf_length) {

  std::string buf_uncompressed;
  if (m_snappy) {
    size_t uncompressed_length;
    snappy::GetUncompressedLength(buf, buf_length, &uncompressed_length);
    buf_uncompressed.resize(uncompressed_length);
    snappy::RawUncompress(buf, buf_length, &buf_uncompressed[0]);
    decodeUncompressedMessage(buf_uncompressed.c_str());
  }
  else {
    decodeUncompressedMessage(buf);
  }
}

void EventData::decodeUncompressedMessage(const char *buf) {
  auto eventData = GetFlatbufEventData(reinterpret_cast<const uint8_t *>(buf));
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

  if (m_snappy) {
    size_t input_len = static_cast<size_t>(builder.GetSize());
    buffer.resize(snappy::MaxCompressedLength(input_len));
    snappy::RawCompress(bufferpointer, input_len, &buffer[0], &m_bufferSize);
  } else {
    buffer.assign(bufferpointer, bufferpointer + builder.GetSize());
    m_bufferSize = builder.GetSize();
  }

  return builder.ReleaseBufferPointer();
}
