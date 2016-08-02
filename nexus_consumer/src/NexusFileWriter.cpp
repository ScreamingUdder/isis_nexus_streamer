#include "NexusFileWriter.h"

using namespace H5;

NexusFileWriter::NexusFileWriter(const std::string &filename)
    : m_file(new H5File(filename, H5F_ACC_TRUNC)) {}
