#include "ProtoEventData.h"

ProtoEventData::ProtoEventData()
    : m_bufferSize(0), m_frameNumber(0), m_numberOfFrames(0){};

ProtoEventData::ProtoEventData(std::string &buffer) {
  decodeMessage(buffer);
}

void ProtoEventData::decodeMessage(std::string &buffer) {

  ProtobufEventData protoEventData;

  protoEventData.ParseFromString(buffer);

  m_numberOfFrames = static_cast<uint32_t>(protoEventData.totalframes());
  m_frameNumber = static_cast<uint32_t>(protoEventData.framenumber());

  m_tof.resize(static_cast<size_t>(protoEventData.tof_size()));
  m_detId.resize(static_cast<size_t>(protoEventData.tof_size()));

  for (int j = 0; j < protoEventData.tof_size(); j++) {
    m_detId[j] = static_cast<uint32_t>(protoEventData.detid(j));
    m_tof[j] = static_cast<uint64_t>(protoEventData.tof(j));
  }
}

bool ProtoEventData::getBufferPointer(std::string &buffer) {

  ProtobufEventData protoEventData;

  uint32_t data_count = getNumberOfEvents();
  protoEventData.set_count(data_count);

  for (uint32_t i = 0; i < data_count; ++i) {
    protoEventData.add_detid(m_detId[i]);
    protoEventData.add_tof(m_tof[i]);
  }
  protoEventData.set_framenumber(m_frameNumber);
  protoEventData.set_totalframes(m_numberOfFrames);

  protoEventData.SerializePartialToString(&buffer);

  return 0;
}

