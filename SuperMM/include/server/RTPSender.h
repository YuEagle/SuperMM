#ifndef _RTP_PACKETIZER_H_
#define _RTP_PACKETIZER_H_

#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "BaseProber.h"
#include "DataBuffer.h"


typedef struct {
        int RTPSocket;
        int RTCPSocket;
        uint32_t RTPPort;
        int32_t TrackIndex;
        struct sockaddr_in mRemoteRTPAddr;
        struct sockaddr_in mRemoteRTCPAddr;
        bool IsEnd;
        int64_t BaseTime;
}SenderInfo;

class RTPSender {
public:
    struct ThreadParam {
        void *param;
        int index;
    };

    RTPSender(BaseProber *prober);
    ~RTPSender();

    SenderInfo getSenderInfo(int32_t index);
    SenderInfo addSenderInfo(int32_t index, int port);
    int recvData(int index);
    static void *recvEntry(void *param);
    int sendData();
    static void *sendEntry(void *param);
    void makePortPair(int *rtpSocket, int *rtcpSocket, uint32_t *rtpPort);

private:
    BaseProber *mBaseProber;

    vector<SenderInfo> mSenderVector;
	int mNALLengthSize;
};

#endif
