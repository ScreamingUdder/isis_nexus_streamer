#ifndef ISIS_NEXUS_STREAMER_EVENTDATA_H
#define ISIS_NEXUS_STREAMER_EVENTDATA_H

#include <cstdint>
#include <vector>

#include "eventDataFlatBuffer_generated.h"

class EventData {

public:
  // Construct a new empty EventData object
  EventData();
  // Construct an EventData object from a flatbuf buffer
  EventData(const char *buf, size_t buf_length);

  // Decode message into existing EventData instance
  void decodeMessage(const char *buf, size_t buf_length);

  // Setters
  void setDetId(std::vector<uint32_t> detIds) { m_detId = detIds; };
  void setTof(std::vector<uint64_t> tofs) { m_tof = tofs; };
  void setNumberOfFrames(uint32_t numberOfFrames) {
    m_numberOfFrames = numberOfFrames;
  };
  void setFrameNumber(uint32_t frameNumber) { m_frameNumber = frameNumber; };

  // Getters
  std::vector<uint32_t> getDetId() { return m_detId; };
  std::vector<uint64_t> getTof() { return m_tof; };
  uint32_t getNumberOfFrames() { return m_numberOfFrames; };
  uint32_t getFrameNumber() { return m_frameNumber; };
  uint32_t getNumberOfEvents() { return m_tof.size(); };

  flatbuffers::unique_ptr_t getBufferPointer(std::string &buffer);

  size_t getBufferSize() { return m_bufferSize; };
  bool getUsingSnappy() { return m_snappy; };

private:
  void decodeUncompressedMessage(const char *buf);
  std::vector<uint32_t> m_detId = {};
  std::vector<uint64_t> m_tof = {};
  uint32_t m_numberOfFrames;
  uint32_t m_frameNumber;
  size_t m_bufferSize;
  bool m_snappy = true;
};

#endif // ISIS_NEXUS_STREAMER_EVENTDATA_H
