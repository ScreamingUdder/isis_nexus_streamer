#ifndef ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
#define ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H

#include <H5Cpp.h>
#include <memory>

// a typedef for our managed H5File pointer
typedef std::shared_ptr<H5::H5File> H5FilePtr;

class NexusFileReader {
public:
  NexusFileReader(const std::string &filename);
  hsize_t getFileSize();
  uint64_t getTotalCounts();
private:
  H5FilePtr m_file = nullptr;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
