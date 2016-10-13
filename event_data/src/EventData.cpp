#include "EventData.h"
#include <iostream>
#include <memory>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <capnp/schema.h>
#include <capnp/dynamic.h>

using ::capnp::DynamicValue;
using ::capnp::DynamicStruct;
using ::capnp::DynamicEnum;
using ::capnp::DynamicList;
using ::capnp::List;
using ::capnp::Schema;
using ::capnp::StructSchema;
using ::capnp::EnumSchema;

using ::capnp::Void;
using ::capnp::MallocMessageBuilder;
using ::capnp::SegmentArrayMessageReader;

EventData::EventData()
    : m_bufferSize(0), m_frameNumber(0), m_numberOfFrames(0), m_totalCounts(0) {
}

EventData::EventData(const uint8_t *buf) { decodeMessage(buf); }

uint64_t getMessageID(const std::string &rawbuf) {

  auto buf = reinterpret_cast<const uint8_t *>(rawbuf.c_str());

  auto wordLimit = capnp::ReaderOptions().traversalLimitInWords;
  // TODO don't forget to clean up this pointer
  auto message_segments = kj::arrayPtr(reinterpret_cast<const capnp::word*>(buf), wordLimit);
  capnp::SegmentArrayMessageReader reader(&message_segments);
  auto messageStruct = reader.getRoot<EventMessage>();
  auto id =  messageStruct.getId();
  delete message_segments;
  return id;
}

void EventData::decodeMessage(const uint8_t *buf) {

  auto wordLimit = capnp::ReaderOptions().traversalLimitInWords;
  // TODO don't forget to clean up this pointer
  auto message_segments = kj::arrayPtr(reinterpret_cast<const capnp::word*>(buf), wordLimit);
  capnp::SegmentArrayMessageReader reader(&message_segments);

  auto messageStruct = reader.getRoot<EventMessage>();
  auto message = messageStruct.getMessage();
  if (message.hasData()) {
    auto eventDataMsg = message.getData();
    auto count = eventDataMsg.getCount();
    m_detId.resize(static_cast<size_t>(count));
    m_tof.resize(static_cast<size_t>(count));
    std::copy(eventDataMsg.getDetId().begin(), eventDataMsg.getDetId().end(), m_detId.begin());
    std::copy(eventDataMsg.getTof().begin(), eventDataMsg.getTof().end(), m_tof.begin());

    setNumberOfFrames(eventDataMsg.getTotalFrames());
    setFrameNumber(eventDataMsg.getFrameNumber());
    setTotalCounts(eventDataMsg.getTotalCounts());
    setEndFrame(eventDataMsg.getFrameEnd());
    setEndRun(eventDataMsg.getRunEnd());

  } else {
    std::cout << "Unrecognised message type" << std::endl;
  }
  delete message_segments;
}

kj::Array<capnp::word> EventData::getBufferPointer(std::string &buffer, uint64_t messageID) {

  MallocMessageBuilder builder;

  auto messageStruct = builder.initRoot<EventMessage>();
  auto eventStruct = messageStruct.initMessage().initData<FlatbufEventData>();

  uint32_t counts = static_cast<uint32_t>(m_detId.size());
  eventStruct.setCount(counts);

  auto detIdsList = eventStruct.initDetId(counts);
  for (uint32_t i=0; i<counts; i++) {
    detIdsList.set(i, m_detId[i]);
  }

  auto tofsList = eventStruct.initTof(counts);
  for (uint32_t i=0; i<counts; i++) {
    tofsList.set(i, m_tof[i]);
  }

  eventStruct.setFrameNumber(m_frameNumber);
  eventStruct.setTotalFrames(m_numberOfFrames);
  eventStruct.setTotalCounts(m_totalCounts);
  eventStruct.setFrameEnd(m_endFrame);
  eventStruct.setRunEnd(m_endRun);

  messageStruct.setId(messageID);

  m_bufferSize = capnp::computeSerializedSizeInWords(builder) * sizeof(capnp::word);
  auto words = capnp::messageToFlatArray(builder);
  auto bytes = words.asBytes();
  buffer.assign(reinterpret_cast<const char *>(bytes.begin()), bytes.size());
  return words;
}
