#ifndef _DATA_BUFFER_H_

#define _DATA_BUFFER_H_

typedef struct{
    uint8_t *data;
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
}DataBuffer;

#endif
