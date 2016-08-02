#ifndef ISIS_NEXUS_STREAMER_NEXUSFILEWRITER_H_H
#define ISIS_NEXUS_STREAMER_NEXUSFILEWRITER_H_H

#include <H5Cpp.h>
#include <memory>
#include <vector>

typedef std::unique_ptr<H5::H5File> H5FilePtr;

class NexusFileWriter {
public:
  NexusFileWriter(const std::string &filename);

private:
  H5FilePtr m_file = nullptr;
};

#endif //ISIS_NEXUS_STREAMER_NEXUSFILEWRITER_H_H
