#ifndef ISIS_NEXUS_STREAMER_MOCKEVENTPUBLISHER_H
#define ISIS_NEXUS_STREAMER_MOCKEVENTPUBLISHER_H

#include "EventPublisher.h"
#include <gmock/gmock.h>

class MockEventPublisher : public EventPublisher {
public:
  MOCK_METHOD2(setUp,
               void(const std::string &broker, const std::string &topic));
};

#endif // ISIS_NEXUS_STREAMER_MOCKEVENTPUBLISHER_H
