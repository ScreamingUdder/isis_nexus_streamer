#include "nexusFileReader.h"

using namespace H5;

H5FilePtr NexusFileReader::openFile(const std::string &filename) {
  return std::make_shared<H5File>(filename, H5F_ACC_RDONLY);
}
