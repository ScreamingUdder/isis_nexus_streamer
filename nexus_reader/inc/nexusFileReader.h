#ifndef ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
#define ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H

#include <H5Cpp.h>
#include <memory>
#include <vector>

// a typedef for our managed H5File pointer
typedef std::shared_ptr<H5::H5File> H5FilePtr;

class NexusFileReader {
public:
  NexusFileReader(const std::string &filename);

  hsize_t getFileSize();
  uint64_t getTotalEventCount();
  bool getEventDetIds(std::vector<uint32_t> &detIds);
  size_t getNumberOfFrames() { return m_numberOfFrames; };

private:
  H5FilePtr m_file = nullptr;
  size_t m_frameNumber = 0;
  size_t m_numberOfFrames;
};

#endif // ISIS_NEXUS_STREAMER_NEXUSFILEREADER_H
