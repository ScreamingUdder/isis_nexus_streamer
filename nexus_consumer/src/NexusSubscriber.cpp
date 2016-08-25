#include <ctime>
#include <iostream>

#include "NexusSubscriber.h"

NexusSubscriber::NexusSubscriber(std::shared_ptr<EventSubscriber> subscriber,
                                 const std::string &brokerAddress,
                                 const std::string &streamName,
                                 const bool quietMode,
                                 const std::string &filename)
    : m_subscriber(subscriber), m_quietMode(quietMode) {
  subscriber->setUp(brokerAddress, streamName);
  if (!filename.empty()) {
    m_filewriter = std::make_shared<NexusFileWriter>(NexusFileWriter(filename));
  }
}

/**
 * Listen for incoming messages from subscribed topic
 */
void NexusSubscriber::listen() {
  auto receivedDataStats = std::make_shared<ReceivedDataStats>();

  std::string message;
  auto receivedData = std::make_shared<EventData>();

  reportProgress(0.0);

  // frame numbers run from 0 to numberOfFrames-1
  while ((receivedDataStats->frameNumber <
          (receivedDataStats->numberOfFrames - 1)) ||
         (receivedDataStats->numberOfmessagesReceived %
          receivedDataStats->messagesPerFrame) != 0) {
    if (!(m_subscriber->listenForMessage(message)))
      continue;

    decodeMessage(receivedData, message, receivedDataStats);
    // TODO should probably give up listening and report an error about a
    // missing message if the map gets too large (64 messages?)
  }
  reportProgress(1.0);
  std::cout << std::endl
            << "Frames received: " << receivedDataStats->numberOfFramesReceived
            << std::endl
            << "Bytes received: " << receivedDataStats->totalBytesReceived
            << std::endl
            << "Number of messages received: "
            << receivedDataStats->numberOfmessagesReceived << std::endl;
}

/**
 * Write the data from the message to file and record statistics such as number
 * of received bytes and frames
 *
 * @param receivedData - EventData object containing the received data
 * @param message - the message buffer, from which to get the number of received
 * bytes
 * @param receivedDataStats - struct of recorded statistics of received data
 */
void NexusSubscriber::processMessage(
    std::shared_ptr<EventData> receivedData, const std::string &message,
    std::shared_ptr<ReceivedDataStats> receivedDataStats) {
  receivedDataStats->totalBytesReceived += message.size();
  receivedDataStats->numberOfFrames = receivedData->getNumberOfFrames();
  receivedDataStats->frameNumber = receivedData->getFrameNumber();
  if (m_filewriter) {
    m_filewriter->writeData(receivedData);
  }
  if (receivedDataStats->frameNumber !=
      receivedDataStats->previousFrameNumber) {
    if (receivedDataStats->frameNumber == 1 &&
        receivedDataStats->messagesPerFrame == 1)
      receivedDataStats->messagesPerFrame =
          receivedDataStats->numberOfmessagesReceived;
    receivedDataStats->numberOfFramesReceived++;
    reportProgress(static_cast<float>(receivedDataStats->frameNumber) /
                   static_cast<float>(receivedDataStats->numberOfFrames));
  }
  receivedDataStats->previousFrameNumber = receivedDataStats->frameNumber;
  receivedDataStats->numberOfmessagesReceived++;
}

/**
 * Decode the received message flatbuffer and ensure messages are processed in
 * order
 *
 * @param eventData
 * @param rawbuf
 * @param receivedDataStats
 */
void NexusSubscriber::decodeMessage(
    std::shared_ptr<EventData> eventData, const std::string &rawbuf,
    std::shared_ptr<ReceivedDataStats> receivedDataStats) {
  // if the message is in order or is the first message then go ahead and decode
  // it
  auto currentMessageID = getMessageID(rawbuf);
  if (currentMessageID == receivedDataStats->previousMessageID + 1 ||
      receivedDataStats->previousMessageID == std::numeric_limits<uint64_t>::max()) {
    eventData->decodeMessage(reinterpret_cast<const uint8_t *>(rawbuf.c_str()));
    processMessage(eventData, rawbuf, receivedDataStats);
    receivedDataStats->previousMessageID = currentMessageID;

    // if this was a message in the map then remove it
    if (m_futureMessages.count(currentMessageID) > 0) {
      m_futureMessages.erase(currentMessageID);
    }

    // if the next messageID is a key in the map then decode it
    if (m_futureMessages.count(currentMessageID + 1) > 0) {
      decodeMessage(eventData, m_futureMessages[currentMessageID + 1],
                    receivedDataStats);
    }
  } else {
    // message is out of order so add it to map to go back to listening
    m_futureMessages[currentMessageID] = rawbuf;
  }
}

/**
 * Display a progress bar
 *
 * @param progress - progress between 0 (starting) and 1 (complete)
 */
void NexusSubscriber::reportProgress(const float progress) {
  if (!m_quietMode) {
    const int barWidth = 70;
    std::cout << "[";
    auto pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
  }
}
