#ifndef ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
#define ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H

#include <H5Cpp.h>
#include <memory>

// a typedef for our managed H5File pointer
typedef std::shared_ptr<H5::H5File> H5FilePtr;

class NexusFileReader {
public:
  NexusFileReader(const std::string &filename);
  NexusFileReader(const std::string &filename, size_t eventSubsetSize);

  hsize_t getFileSize();
  uint64_t getTotalEventCount();
private:
  H5FilePtr m_file = nullptr;
  size_t m_maxEventSubsetSize;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
