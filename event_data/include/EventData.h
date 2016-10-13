#ifndef ISIS_NEXUS_STREAMER_EVENTDATA_H
#define ISIS_NEXUS_STREAMER_EVENTDATA_H

#include <cstdint>
#include <vector>
#include <string>

#include "eventData.capnp.h"

uint64_t getMessageID(const std::string &rawbuf);

class EventData {

public:
  // Construct a new empty EventData object
  EventData();
  // Construct an EventData object from a message buffer
  EventData(const uint8_t *buf);

  // Decode message into existing EventData instance
  void decodeMessage(const uint8_t *buf);

  // Setters
  void setDetId(std::vector<uint32_t> detIds) { m_detId = detIds; }
  void setTof(std::vector<uint64_t> tofs) { m_tof = tofs; }
  void setNumberOfFrames(uint32_t numberOfFrames) {
    m_numberOfFrames = numberOfFrames;
  };
  void setFrameNumber(uint32_t frameNumber) { m_frameNumber = frameNumber; }
  void setTotalCounts(uint64_t totalCounts) { m_totalCounts = totalCounts; }
  void setEndFrame(bool lastInFrame) { m_endFrame = lastInFrame; }
  void setEndRun(bool lastInRun) { m_endRun = lastInRun; }

  // Getters
  std::vector<uint32_t> getDetId() { return m_detId; }
  std::vector<uint64_t> getTof() { return m_tof; }
  uint32_t getNumberOfFrames() { return m_numberOfFrames; }
  uint32_t getFrameNumber() { return m_frameNumber; }
  uint32_t getNumberOfEvents() { return m_tof.size(); }
  uint64_t getTotalCounts() { return m_totalCounts; }
  bool getEndFrame() { return m_endFrame; }
  bool getEndRun() { return m_endRun; }

  kj::Array<capnp::word> getBufferPointer(std::string &buffer, uint64_t messageID);

  size_t getBufferSize() { return m_bufferSize; }

private:
  std::vector<uint32_t> m_detId = {};
  std::vector<uint64_t> m_tof = {};
  uint32_t m_numberOfFrames;
  uint32_t m_frameNumber;
  uint64_t m_totalCounts;
  size_t m_bufferSize;
  bool m_endFrame = false;
  bool m_endRun = false;
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATA_H
