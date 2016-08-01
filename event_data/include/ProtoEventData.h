#ifndef ISIS_NEXUS_STREAMER_PROTOEVENTDATA_H
#define ISIS_NEXUS_STREAMER_PROTOEVENTDATA_H

#include <cstdint>
#include <vector>
#include "eventDataProtoBuffer.pb.h"

class ProtoEventData {
public:
  // Construct a new empty EventData object
  ProtoEventData();
  // Construct an EventData object from a proto buffer
  ProtoEventData(std::string &buffer);

  // Decode message into existing EventData instance
  void decodeMessage(std::string &buffer);

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

  bool getBufferPointer(std::string &buffer);

  size_t getBufferSize() { return m_bufferSize; };

private:
  std::vector<uint32_t> m_detId = {};
  std::vector<uint64_t> m_tof = {};
  uint32_t m_numberOfFrames;
  uint32_t m_frameNumber;
  size_t m_bufferSize;

};

#endif //ISIS_NEXUS_STREAMER_PROTOEVENTDATA_H
