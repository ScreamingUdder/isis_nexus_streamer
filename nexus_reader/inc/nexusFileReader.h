#ifndef ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
#define ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H

#include <H5Cpp.h>
#include <memory>

// a typedef for our managed H5File pointer
typedef std::shared_ptr<H5::H5File> H5FilePtr;

class NexusFileReader {
public:
  H5FilePtr openFile(const std::string &filename);
};

#endif // ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
