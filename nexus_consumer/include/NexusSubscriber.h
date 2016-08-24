#ifndef ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H

#include <memory>
#include <unordered_map>

#include "EventData.h"
#include "EventSubscriber.h"
#include "NexusFileWriter.h"

struct ReceivedDataStats {
  int32_t frameNumber = 0;
  int32_t numberOfFrames = 2;
  int64_t totalBytesReceived = 0;
  int32_t numberOfmessagesReceived = 0;
  int32_t previousFrameNumber = std::numeric_limits<int32_t>::max();
  int numberOfFramesReceived = 0; // to check no messages lost
  int messagesPerFrame = 1;
};

class NexusSubscriber {
public:
  NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                  const std::string &brokerAddress,
                  const std::string &streamName, const bool quietMode,
                  const std::string &filename);

  void listen();
  void decodeMessage(std::shared_ptr<EventData> eventData,
                     const std::string &rawbuf,
                     std::shared_ptr<ReceivedDataStats> receivedDataStats);

private:
  std::shared_ptr<EventSubscriber> m_subscriber;
  std::shared_ptr<NexusFileWriter> m_filewriter;
  bool m_quietMode = false;
  uint64_t m_previousMessageID = std::numeric_limits<uint64_t>::max();
  std::unordered_map<uint64_t, std::string> m_futureMessages;

  void processMessage(std::shared_ptr<EventData> receivedData,
                      const std::string &message,
                      std::shared_ptr<ReceivedDataStats> receivedDataStats);
  void reportProgress(const float progress);
};

#endif // ISIS_NEXUS_STREAMER_NEXUSSUBSCRIBER_H
