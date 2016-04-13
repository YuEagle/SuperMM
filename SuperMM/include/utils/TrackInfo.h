#ifndef _TRACK_INFO_H_
#define _TRACK_INFO_H_

#include <string>
#include <list>
#include "DataBuffer.h"

using namespace std;

struct FrameInfo {
    uint64_t offset;
    size_t size;

    int64_t timestamp;
    // frame type for avc, 
    // 0=AVC sequence header;
    // 1 = AVC NALU
    // 2 = AVC end of sequence
    int8_t AVCPacketType;
    // frame type for aac
    // 0 = AAC sequence header
    // 1 = AAC raw
    int8_t AACPacketType;
};

struct TrackInfo {
    int TrackFlag;
    int TrackIndex;
    string format;

    list<FrameInfo> mFrameList;    
    list<FrameInfo>::iterator mIt;
};

#endif
