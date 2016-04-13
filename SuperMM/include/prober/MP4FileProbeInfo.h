
#ifndef _MP4_FILE_PROBE_INFO_H_

#define _MP4_FILE_PROBE_INFO_H_

#include "BaseProber.h"
#include "FileDataReader.h"

using namespace std; 

class MP4FileProbeInfo : public BaseProber {
public:

    MP4FileProbeInfo(FileDataReader *reader);
    ~MP4FileProbeInfo();

    virtual void FileParse();
    virtual void InfoDump();

    bool parseChunk(uint64_t *offset, uint32_t depth);

private:
    FileDataReader *mFileDataReader;
};


BaseProber *ProbeMP4(FileDataReader *reader);


#endif
