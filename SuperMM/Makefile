TARGET = SuperMM
FLAGS = -g -Wall -lpthread 

#MACROS = -DIS_X86_32BIT

SRC_FILES = Super_Main.cpp \
			src/utils/Utils.cpp \
			src/utils/BitsReader.cpp \
			src/utils/MPEG4AudioConfig.cpp \
			src/utils/AVCVideoConfig.cpp \
			src/prober/BaseProber.cpp \
			src/prober/Register.cpp \
			src/prober/FileDataReader.cpp \
			src/prober/MP3FileProbeInfo.cpp \
			src/prober/MP4FileProbeInfo.cpp \
			src/prober/APEFileProbeInfo.cpp \
			src/prober/AVIFileProbeInfo.cpp \
			src/prober/FLVFileProbeInfo.cpp \
			src/prober/MKVFileProbeInfo.cpp \
			src/prober/ASFFileProbeInfo.cpp \
			src/prober/WAVEFileProbeInfo.cpp \
			src/prober/FLACFileProbeInfo.cpp \
			src/streaming/RTSPDataReader.cpp \
			src/streaming/SessionDescription.cpp \
			src/streaming/RTPConnection.cpp \
			src/server/RTSPServer.cpp \
			src/server/SDPData.cpp \
			src/server/RTPSender.cpp \
			src/server/Packetizer.cpp \

INC_PATH = -I include \
		   -I include/streaming \
		   -I include/prober \
		   -I include/utils \
		   -I include/server \

#LIB_PATH = \
		   -L ../utils \

#LIBS = \
	   -lutils\

all :
	g++ $(SRC_FILES) -o ${TARGET} ${FLAGS} ${INC_PATH} 

#${LIB_PATH} ${LIBS}

clean:
	rm -rf ${TARGET} *.o
