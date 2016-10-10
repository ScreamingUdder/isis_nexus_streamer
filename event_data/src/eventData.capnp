@0xf79c074e0ae359d2;

# IDL file for neutron event data.

struct FlatbufEventData {
    count @0 :UInt32;
    detId @1 :List(UInt32);
    tof @2 :List(UInt64);
    frameNumber @3 :UInt32;
    totalFrames @4 :UInt32;
    totalCounts @5 :UInt64;
    frameEnd @6 :Bool = false;
    runEnd @7 :Bool = false;
}

struct EventMessage {
    message :union {
        nothing @0 :Void;
        data @1 :FlatbufEventData;
    }
    id @2 :UInt64;
}
