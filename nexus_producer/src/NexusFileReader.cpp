#include "NexusFileReader.h"
#include <iostream>

using namespace H5;

/**
 * Create a object to read the specified file
 *
 * @param filename - the full path of the NeXus file
 * @return - an object with which to read information from the file
 */
NexusFileReader::NexusFileReader(const std::string &filename)
    : m_file(std::make_shared<H5File>(filename, H5F_ACC_RDONLY)) {
  DataSet dataset = m_file->openDataSet("/raw_data_1/good_frames");
  size_t *numOfFrames = new size_t[1];
  dataset.read(numOfFrames, PredType::NATIVE_UINT64);
  m_numberOfFrames = *numOfFrames;
  // Reduce number of frames by 1, this is a fix for an inconsistency between
  // the SANS and WISH files
  m_numberOfFrames--;
  delete[] numOfFrames;
}

/**
 * Get the size of the NeXus file in bytes
 *
 * @return - the size of the file in bytes
 */
hsize_t NexusFileReader::getFileSize() { return m_file->getFileSize(); }

uint64_t NexusFileReader::getTotalEventCount() {
  DataSet dataset =
      m_file->openDataSet("/raw_data_1/detector_1_events/total_counts");
  uint64_t *totalCount = new uint64_t[1];
  dataset.read(totalCount, PredType::NATIVE_UINT64);

  return *totalCount;
}

/**
 * Gets the event index of the start of the specified frame
 *
 * @param frameNumber - find the event index for the start of this frame
 * @return - event index corresponding to the start of the specified frame
 */
hsize_t NexusFileReader::getFrameStart(hsize_t frameNumber) {
  auto dataset =
      m_file->openDataSet("/raw_data_1/detector_1_events/event_index");
  uint64_t *frameStart = new uint64_t[1];

  hsize_t count[1], offset[1], stride[1], block[1];
  count[0] = 1;
  offset[0] = frameNumber;
  stride[0] = 1;
  block[0] = 1;

  auto dataspace = dataset.getSpace();
  dataspace.selectHyperslab(H5S_SELECT_SET, count, offset, stride, block);

  hsize_t dimsm[1];
  dimsm[0] = 1;
  DataSpace memspace(1, dimsm);

  dataset.read(frameStart, PredType::NATIVE_UINT64, memspace, dataspace);

  return *frameStart;
}

/**
 * Get the number of events which are in the specified frame
 *
 * @param frameNumber - the number of the frame in which to count the number of events
 * @return - the number of events in the specified frame
 */
hsize_t NexusFileReader::getNumberOfEventsInFrame(hsize_t frameNumber) {
  return getFrameStart(frameNumber + 1) - getFrameStart(frameNumber);
}

/**
 * Get the list of detector IDs corresponding to events in the specifed frame
 *
 * @param detIds - vector in which to store the detector IDs
 * @param frameNumber - the number of the frame in which to get the detector IDs
 * @return - false if the specified frame number is not the data range, true otherwise
 */
bool NexusFileReader::getEventDetIds(std::vector<uint32_t> &detIds,
                                     hsize_t frameNumber) {
  if (frameNumber >= m_numberOfFrames)
    return false;
  auto dataset =
      m_file->openDataSet("/raw_data_1/detector_1_events/event_id");

  auto numberOfEventsInFrame = getNumberOfEventsInFrame(frameNumber);

  hsize_t count[1], offset[1], stride[1], block[1];
  count[0] = numberOfEventsInFrame;
  offset[0] = getFrameStart(frameNumber);
  stride[0] = 1;
  block[0] = 1;

  auto dataspace = dataset.getSpace();
  dataspace.selectHyperslab(H5S_SELECT_SET, count, offset, stride, block);

  uint32_t *detIdsArray = new uint32_t[numberOfEventsInFrame];

  hsize_t dimsm[1];
  dimsm[0] = numberOfEventsInFrame;
  DataSpace memspace(1, dimsm);

  dataset.read(detIdsArray, PredType::NATIVE_UINT32, memspace, dataspace);
  detIds.insert(detIds.end(), &detIdsArray[0],
                &detIdsArray[numberOfEventsInFrame]);

  return true;
}

/**
 * Get the list of flight times corresponding to events in the specifed frame
 *
 * @param tofs - vector in which to store the time-of-flight
 * @param frameNumber - the number of the frame in which to get the time-of-flights
 * @return - false if the specified frame number is not the data range, true otherwise
 */
bool NexusFileReader::getEventTofs(std::vector<uint64_t> &tofs,
                                   hsize_t frameNumber) {
  if (frameNumber >= m_numberOfFrames)
    return false;
  auto dataset =
      m_file->openDataSet("/raw_data_1/detector_1_events/event_time_offset");

  auto numberOfEventsInFrame = getNumberOfEventsInFrame(frameNumber);

  hsize_t count[1], offset[1], stride[1], block[1];
  count[0] = numberOfEventsInFrame;
  offset[0] = getFrameStart(frameNumber);
  stride[0] = 1;
  block[0] = 1;

  auto dataspace = dataset.getSpace();
  dataspace.selectHyperslab(H5S_SELECT_SET, count, offset, stride, block);

  double *timeOffsetArray = new double[numberOfEventsInFrame];

  hsize_t dimsm[1];
  dimsm[0] = numberOfEventsInFrame;
  DataSpace memspace(1, dimsm);

  dataset.read(timeOffsetArray, PredType::NATIVE_DOUBLE, memspace, dataspace);

  tofs.resize(numberOfEventsInFrame);

  for (size_t tofIndex = 0; tofIndex < numberOfEventsInFrame; tofIndex++) {
    tofs[tofIndex] = static_cast<uint64_t>((timeOffsetArray[tofIndex] * 1e3));
  }
  delete[] timeOffsetArray;

  return true;
}
