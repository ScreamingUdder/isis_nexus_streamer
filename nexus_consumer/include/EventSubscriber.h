#ifndef ISIS_NEXUS_STREAMER_EVENTSUBSCRIBER_H
#define ISIS_NEXUS_STREAMER_EVENTSUBSCRIBER_H

#include <string>

class EventSubscriber {
public:
  virtual ~EventSubscriber() {}
  virtual void setUp(const std::string &broker, const std::string &topic) = 0;
  virtual bool listenForMessage(std::string &message) = 0;
};

#endif //ISIS_NEXUS_STREAMER_EVENTSUBSCRIBER_H
