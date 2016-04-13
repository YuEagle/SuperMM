#include <iostream>
#include <stdio.h>
#include <string.h>
#include "Register.h"
#include "FileDataReader.h"
#include "RTSPDataReader.h"
#include "RTSPServer.h"

using namespace std; 

int main(int argc,char *argv[]) {

    if (!strncmp(argv[1], "server", 6)) {
        printf("Start server ! \n");
        startServer();
    } else {
        const char *url = argv[1];
        printf("file name :%s\n",argv[1]);

        FileDataReader *filereader = NULL;
        Register *reg = NULL;
        BaseProber *prober = NULL;
        RTSPDataReader *streamreader = NULL;

        if (!strncmp(url, "rtsp://", 7)) {
            streamreader = new RTSPDataReader(url);

            delete streamreader;
        } else {
            filereader = new FileDataReader(url); //New one FileDataReader instance.

            reg = new Register(filereader); //New one Register instance.

            prober = reg->probe();
            if (prober == NULL) {
                printf("Have not found the required prober !\n");
                return -1;

                delete filereader;
                delete reg;
            }

            prober->FileParse();
            prober->InfoDump();
            delete filereader;
            delete reg;
        }
    }
    return 0;
}
