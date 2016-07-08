import h5py

"""
Read data from compressed nexus file and write it to a new uncompressed one.
"""
with h5py.File('SANS_test.nxs', 'r') as f_read:
    data_1 = f_read.get('/raw_data_1/detector_1_events/event_frame_number')

    with h5py.File('SANS_test_uncompressed.hdf5', 'w') as f_write:
        print(data_1[...])
        g = f_write.create_group('/raw_data_1/detector_1_events')
        g['event_frame_number'] = data_1[...]
