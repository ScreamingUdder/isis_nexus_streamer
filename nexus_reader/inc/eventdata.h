#ifndef ISIS_NEXUS_STREAMER_EVENTDATA_H
#define ISIS_NEXUS_STREAMER_EVENTDATA_H

#include <vector>
#include <cstdint>


class EventData {

public:
    std::vector<int32_t> getDetid();
    std::vector<float> getTof();

private:
    std::vector<int32_t> detid = {};
    std::vector<float> tof = {};

};

#endif //ISIS_NEXUS_STREAMER_EVENTDATA_H
