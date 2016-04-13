#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "RTPSender.h"
#include "Utils.h"
#include "Packetizer.h"

RTPSender::RTPSender(BaseProber *prober)
    :mBaseProber(prober),
	 mNALLengthSize(-1){
}

RTPSender::~RTPSender() {

}

SenderInfo RTPSender::getSenderInfo(int32_t index) {
    vector<SenderInfo>::iterator it = mSenderVector.begin();
    while (it != mSenderVector.end()) {
        if (it->TrackIndex == index) {
            break;
        }
        it++;
    }
    return *it;
}

SenderInfo RTPSender::addSenderInfo(int32_t index, int port) {
    SenderInfo info;
    info.TrackIndex = index;
//printf("0000000000000000000000000000000000000000 index %d\n", index);
    makePortPair(&info.RTPSocket, &info.RTCPSocket, &info.RTPPort);

    info.mRemoteRTPAddr.sin_family = AF_INET;
    info.mRemoteRTPAddr.sin_addr.s_addr = inet_addr("192.168.1.103");
    info.mRemoteRTPAddr.sin_port = htons(port);
	printf("info.mRemoteRTPAddr.sin_port %d,  info.RTPPort %d\n", info.mRemoteRTPAddr.sin_port, info.RTPPort);

    info.mRemoteRTCPAddr.sin_family = AF_INET;
    info.mRemoteRTCPAddr.sin_addr.s_addr = inet_addr("192.168.1.103");
    info.mRemoteRTCPAddr.sin_port = htons(port + 1);

    printf("rtcp port %d\n", port + 1);
    info.IsEnd = false;
    info.BaseTime = -1;

    mSenderVector.push_back(info);

    TrackInfo *trackinfo = mBaseProber->getTrackInfo(index);

	if (!strncmp(trackinfo->format.c_str(), "video/avc", 9)) {
		DataBuffer *buf = mBaseProber->getVideoConfigBuffer();
		uint8_t *data = buf->data;
		mNALLengthSize = 1 + (data[4] & 3);
		printf("---%x %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4], data[5]);
		printf("mNALLengthSize %d \n", mNALLengthSize);	
	}

    return info;
}

int RTPSender::recvData(int index) {
    ThreadParam *tp = new ThreadParam();
    tp->param = (void *)this;
    tp->index = index;

    pthread_t tid;
    int err = pthread_create(&tid, NULL, recvEntry, (void*)tp);

    if (err) {
        return -1;
    }

    return 0;
}

void *RTPSender::recvEntry(void *param) {
    ThreadParam *tp = (ThreadParam  *)param; 
    RTPSender *sender = (RTPSender *)(tp->param);
    int i = tp->index;

    SenderInfo info = sender->mSenderVector[i];

    while (1) {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000ll;

        fd_set rs;
        FD_ZERO(&rs);
        int maxSocket = -1;
        FD_SET(info.RTPSocket, &rs);
        FD_SET(info.RTCPSocket, &rs);

        if (info.RTPSocket > maxSocket) {
            maxSocket = info.RTPSocket;
        }
        if (info.RTCPSocket > maxSocket) {
            maxSocket = info.RTCPSocket;
        }
        int res = select(maxSocket + 1, &rs, NULL, NULL, &tv);
//        printf("select --------------   res %d\n", res);

        int n = -1;
        if (res > 0) {
            if (FD_ISSET(info.RTPSocket, &rs)) {
                uint8_t buf[65536];
                size_t size = 65536;
                struct sockaddr_in *from = &(info.mRemoteRTPAddr);
                socklen_t socklen = socklen_t(sizeof(sockaddr));
                n = recvfrom(info.RTPSocket, buf, size, 0, (struct sockaddr *)from, &socklen);
                printf("rtp recv %d bytes\n", n);
            }

            if (n > 0 && FD_ISSET(info.RTCPSocket, &rs)) {
                uint8_t buf[65536];
                size_t size = 65536;
                struct sockaddr_in *from = &(info.mRemoteRTCPAddr);
                socklen_t socklen = socklen_t(sizeof(sockaddr));
                n = recvfrom(info.RTCPSocket, buf, size, 0, (struct sockaddr *)from, &socklen);
                printf("rtcp recv %d bytes\n", n);
            }
        }
    
   //     printf("--------  recv data \n");
    }
}

int RTPSender::sendData() {
    //Create pthread here.

    int num = mSenderVector.size();
    pthread_t tid[num];
    for (int i = 0; i <  num; i++) {
        ThreadParam *tp = new ThreadParam();
        tp->param = (void *)this;
        tp->index = i;

        int err = pthread_create(&tid[i], NULL, sendEntry, (void*)tp);

        if (err) {
            return -1;
        }
    }
    return 0;
}

static int64_t getSystemTime() {
    int64_t systime;
    struct timeval time;
    gettimeofday(&time, NULL);

    systime = time.tv_sec * 1000000 + time.tv_usec;

    return systime;
}

//static
void *RTPSender::sendEntry(void *param) {
    ThreadParam *tp = (ThreadParam  *)param; 
    RTPSender *pack = (RTPSender *)(tp->param);
    int index = tp->index;

    int64_t basetime = getSystemTime();
    SenderInfo *senderinfo = NULL;
    for (int k = 0; k < pack->mSenderVector.size(); k++) {
	if (index == pack->mSenderVector[k].TrackIndex) {
		senderinfo = &pack->mSenderVector[k];
	}
    }
	
    TrackInfo *trackinfo = pack->mBaseProber->getTrackInfo(senderinfo->TrackIndex);

    string format(trackinfo->format);
    Packetizer *packetizer = new Packetizer(format.c_str(), senderinfo->TrackIndex);

	if (!strncmp(format.c_str(), "video/avc", 9)) {
		packetizer->setNALLengthSize(pack->mNALLengthSize);
	}

    while (1) {
        DataBuffer *buff = new DataBuffer();
        if (pack->mBaseProber->readData(senderinfo->TrackIndex, buff) == -1) {
            senderinfo->IsEnd = true;

            struct sockaddr_in *to = &(senderinfo->mRemoteRTCPAddr);

            const char *str = "BYE123456789qazxswedfftttg\0";
            printf("%s\n", str);

            int err = sendto(senderinfo->RTCPSocket, str, strlen(str), 0, (const struct sockaddr *)to, sizeof(sockaddr));
            printf("send to rtcp err %d\n", err);

            delete buff;
            break;
        }

        if (senderinfo->BaseTime < 0) {
            senderinfo->BaseTime = buff->timestamp;
        }

        packetizer->packetize(buff);

        int64_t currtime = getSystemTime();
        int64_t diff1 = currtime - basetime;
        int64_t diff2 = buff->timestamp - senderinfo->BaseTime;

        if (diff2 > diff1) {
            usleep(diff2 - diff1);
        }

        printf("buff size %d stream  index %d diff1 %ld, diff2 %ld\n", (int)buff->size, senderinfo->TrackIndex, diff1, diff2);
  
        struct sockaddr_in *to = &(senderinfo->mRemoteRTPAddr);

        uint8_t data[4096 * 4];
        uint8_t *tmp = data;
        size_t size = 0;
        while (!packetizer ->getPacketData(tmp, &size)) {
            int n = sendto(senderinfo->RTPSocket, data, size, 0, (const struct sockaddr *)to, sizeof(sockaddr));
//			printf("----send bytes %d port %d\n", n, to->sin_port);
        }

        free(buff->data);
        delete buff;
    }

    delete packetizer;

    return NULL;
}

void RTPSender::makePortPair(int *rtpSocket, int *rtcpSocket, uint32_t *rtpPort) {
    *rtpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    int size = 256 * 1024;
    setsockopt(*rtpSocket, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    *rtcpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    size = 256 * 1024;
    setsockopt(*rtcpSocket, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    /* rand() * 1000 may overflow int type, use long long */
    //unsigned start = (rand() * 1000)/RAND_MAX + 15550;
    unsigned randPart = (unsigned)((rand()* 1000ll)/RAND_MAX);
    unsigned start = 15550+randPart; /* variable @start: default start port */
    unsigned startPort = start;
    unsigned endPort = 65536; /*variable @endPort: default end port is 65536 */

    if(startPort>endPort){
        startPort = start;
        endPort = 65536;
    }
    startPort &= ~1;

    for (unsigned port = startPort; port < endPort; port += 2) {
        struct sockaddr_in addr;
        memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        if (bind(*rtpSocket, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
            continue;
        }

        addr.sin_port = htons(port + 1);

        if (bind(*rtcpSocket, (const struct sockaddr *)&addr, sizeof(addr)) == 0) {
            *rtpPort = port;
            return;
        }
    }
}
