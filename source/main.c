#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/iosupport.h>
#include "services.h"
#include "minisoc.h"
#include "mythread.h"

#define MAX_SESSIONS 1
#define SERVICE_ENDPOINTS 3
#define CRASH *(u32*)__LINE__ = 0xFF;

static Result should_terminate(int *term_request) {
    u32 notid;

    Result ret = srvReceiveNotification(&notid);
    if (R_FAILED(ret)) {
        return ret;
    }
    if (notid == 0x100) {// term request
        *term_request = 1;
    }
    return 0;
}

// this is called before main
void __appInit() {
    srvSysInit();
    
    if(cfguInit() > 0)		
        CRASH;
    //consoleDebugInit(debugDevice_SVC);
    //gdbHioDevInit();
    //gdbHioDevRedirectStdStreams(false, true, false);
    if(miniSocInit() > 0)
        CRASH;
}

// this is called after main exits
void __appExit() {
    miniSocExit();
    cfguExit();
    srvSysExit();
}

// stubs for non-needed pre-main functions
void __sync_init();
void __sync_fini();
void __system_initSyscalls();
void __libc_init_array(void);
void __libc_fini_array(void);

void initSystem(void (*retAddr)(void)) {
    __libc_init_array();
    __sync_init();
    __system_initSyscalls();
    __appInit();
}

void __ctru_exit(int rc) {
    __appExit();
    __sync_fini();
    __libc_fini_array();
    svcExitProcess();
}

static u8 tag_state = 0;
static Handle events[2] = {-1, -1};
static u8 ALIGN(8) threadStack[0x1000] = {0};
static u8 ALIGN(8) statbuf[0x800] = {0};
static int sockfd = -1;
static int connfd = -1;
static struct sockaddr_in cli; 

typedef struct 
{
    LightEvent lockEvent;
    LightEvent ReadCompleteEvent;
    u8 *buf;
    MyThread thread;
    char done;
    char connected;
} sockThreadStruct;

void sockrwThread(void *arg)
{
    sockThreadStruct *data = (sockThreadStruct*) arg;
    Result ret = 0;
    struct pollfd fds[1];
    int nfds = 1;

    if(connfd == -1)
    {
        size_t len = sizeof(cli); 
        while(1)
        {
            if(data->done) break;
            memset(fds, 0, sizeof(fds));
            fds[0].fd = sockfd;
            fds[0].events = POLLIN;
            ret = socPoll(fds, nfds, 50);
            if(ret < 0) 
                MyThread_Exit();
            
            if(ret > 0)
            {
                if(fds[0].revents & POLLIN)
                {
                    connfd = socAccept(sockfd, &cli, &len); 
                    if(connfd < 0) 
                        CRASH;
                    
                    data->connected = true;
                    break;
                }
            }
        }
    }

    while(1)
    {
        LightEvent_Wait(&data->lockEvent);
        if(data->done) break;
        socSend(connfd, data->buf, 256, 0);
        socRecv(connfd, &data->buf[0], 256, 0);
        LightEvent_Signal(&data->ReadCompleteEvent);
    }
    MyThread_Exit();
}

void sockSendRecvData(sockThreadStruct *data, u32 *cmdbuf)
{
    memcpy(&data->buf[0], (u8*)&cmdbuf[0], 256);
    LightEvent_Signal(&data->lockEvent);
    //printf("Waiting for read to complete\n");
    LightEvent_Wait(&data->ReadCompleteEvent);
    //printf("cmdheader %08X\n", (u32*)data->buf[0]);
    memcpy((u8*)&cmdbuf[0], &data->buf[0], 256);
            
}

void handle_commands(sockThreadStruct *data)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    u16 cmdid = cmdbuf[0] >> 16;
    //printf("Cmdid %x called\n", cmdid);
    //This is a bare-minimum ipc-handler for some critical funcs to ensure that stuff isn't broken when 
    //the companion isn't connected
    switch(cmdid)
    {
        case 1: //Initialize
        {	
            data->done = 0;
            MyThread_Create(&data->thread, &sockrwThread, (void*)data, threadStack, 0x1000, 15, -2);
            tag_state = NFC_TagState_ScanningStopped;
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 2: //Finalize
        {
            data->done = 1;
            socClose(sockfd);
            LightEvent_Signal(&data->lockEvent); // Signal LightEvent since the thread is waiting for it
            MyThread_Join(&data->thread, 2e+9);
            tag_state = NFC_TagState_Uninitialized;
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 3: // StartCommunication
        {
    //		printf("StartCommunication\n");
            // Now server is ready to listen and verification
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 4: // StopCommunication
        {
            //socClose(connfd);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 5:
        {
            if(events[0] != -1)
                svcSignalEvent(events[0]);
            sockSendRecvData(data, cmdbuf);
            break;
        }

        case 6:
        {
            if(events[1] != -1)
                 svcSignalEvent(events[1]);
            sockSendRecvData(data, cmdbuf);
            break;
        }

        case 0xB: //GetTagInRangeEvent
        {
            svcCreateEvent(&events[0], RESET_ONESHOT);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 2);
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            cmdbuf[3] = events[0];
            break;
        }

        case 0xC: //GetTagOutOfRangeEvent
        {
            svcCreateEvent(&events[1], RESET_ONESHOT);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 2);
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            cmdbuf[3] = events[1];
            break;
        }

        case 0xD:
        {
            if(data->connected)
            {
                sockSendRecvData(data, cmdbuf);
                break;
            }

            cmdbuf[0] = IPC_MakeHeader(cmdid, 2, 0);
            cmdbuf[1] = 0;
            cmdbuf[2] = tag_state;
            break;
        }

        case 0x14: // InitizeAppData
        {
            sockSendRecvData(data, cmdbuf);
            u32 buf_ptr = cmdbuf[0x12];
            FS_ProgramInfo info;
            FSUSER_GetProgramLaunchInfo(&info, cmdbuf[16]);
            memcpy((u8*)&cmdbuf[0], (u8*)&info.programId, 8);                                                                                                                                                          ;
            memcpy((u8*)&cmdbuf[2], buf_ptr, 0xD8);
            sockSendRecvData(data, cmdbuf);
            break;
        }

        case 0x15: // ReadAppData we need to do it here because staticbuf
        {
            sockSendRecvData(data, cmdbuf);
            memcpy(&statbuf, (u8*)&cmdbuf[3], 0xD8);
            cmdbuf[3] = &statbuf;
            break;
        } 

        case 0x16: // WriteAppData because same reason
        {
            // first we send over cmdbuf[0] to cmdbuf[10]
            sockSendRecvData(data, cmdbuf);
            u32 buf_ptr = cmdbuf[11];
            memcpy((u8*)&cmdbuf[0], buf_ptr, 0xD8); //maximum size 0xd8
            sockSendRecvData(data, cmdbuf);
            break;
        }

        case 0x404: // We need to send some additional stuff
        {
            u32 countrycode;
            memcpy(&data->buf[0], (u8*)&cmdbuf[0], 0x100); // use data->buf as a temporary backup point
            Result ret = CFGU_GetConfigInfoBlk2(4, 0xB0000u, &countrycode);
            if(ret > 0)
                CRASH;
            memcpy((u8*)&cmdbuf[0], &data->buf[0], 0x100); // restore data
            cmdbuf[43] = countrycode;
            sockSendRecvData(data, cmdbuf);
            break;
        }

        default:
        {
            if(connfd >= 0)
            {
                sockSendRecvData(data, cmdbuf);	
            }
        }
        
    }

}

int main() {

    int nmbActiveHandles;

    Handle *hndNfuU;
    Handle *hndNfuM;
    Handle *hndNotification;
    Handle hndList[MAX_SESSIONS+SERVICE_ENDPOINTS];

    hndNotification = &hndList[0];
    hndNfuU = &hndList[1];
    hndNfuM = &hndList[2];
    nmbActiveHandles = SERVICE_ENDPOINTS;
    sockThreadStruct thread_data;

    u32* staticbuf = getThreadStaticBuffers();
    staticbuf[0]  = IPC_Desc_StaticBuffer(0x800, 0);
    staticbuf[1]  = &statbuf;
    staticbuf[2]  = IPC_Desc_StaticBuffer(0x800, 0);
    staticbuf[3]  = &statbuf;
    staticbuf[4]  = IPC_Desc_StaticBuffer(0x800, 0);
    staticbuf[5]  = &statbuf;

    u8 buf[256];
    thread_data.buf = buf;
    LightEvent_Init(&thread_data.lockEvent, RESET_ONESHOT);
    LightEvent_Init(&thread_data.ReadCompleteEvent, RESET_ONESHOT);

    Result ret = 0;
    if (R_FAILED(srvRegisterService(hndNfuU, "nfc:u", MAX_SESSIONS))) {
        svcBreak(USERBREAK_ASSERT);
    }
    if (R_FAILED(srvRegisterService(hndNfuM, "nfc:m", MAX_SESSIONS))) {
        svcBreak(USERBREAK_ASSERT);
    }
    if (R_FAILED(srvEnableNotification(hndNotification))) {
        svcBreak(USERBREAK_ASSERT);
    }
    
    struct sockaddr_in servaddr;
    sockfd = socSocket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = socGethostid(); 
    servaddr.sin_port = htons(8001); 
  
    // Binding newly created socket to given IP and verification 
    if ((socBind(sockfd, &servaddr, sizeof(servaddr))) != 0)
        CRASH;
    
    if ((socListen(sockfd, 1)) != 0)
        CRASH;
    
    Handle reply_target = 0;
    int term_request = 0;
    do {
        if (reply_target == 0) {
            u32 *cmdbuf = getThreadCommandBuffer();
            cmdbuf[0] = 0xFFFF0000;
        }
        s32 request_index;
        //logPrintf("B SRAR %d %x\n", request_index, reply_target);
        ret = svcReplyAndReceive(&request_index, hndList, nmbActiveHandles, reply_target);
        //logPrintf("A SRAR %d %x\n", request_index, reply_target);

        if (R_FAILED(ret)) {
            // check if any handle has been closed
            if (ret == 0xC920181A) {
                if (request_index == -1) {
                    for (int i = SERVICE_ENDPOINTS; i < MAX_SESSIONS+SERVICE_ENDPOINTS; i++) {
                        if (hndList[i] == reply_target) {
                            request_index = i;
                            break;
                        }
                    }
                }
                svcCloseHandle(hndList[request_index]);
                hndList[request_index] = hndList[nmbActiveHandles-1];
                nmbActiveHandles--;
                reply_target = 0;
            } else {
                svcBreak(USERBREAK_ASSERT);
            }
        } else {
            // process responses
            reply_target = 0;
            switch (request_index) {
                case 0: { // notification
                    if (R_FAILED(should_terminate(&term_request))) {
                        svcBreak(USERBREAK_ASSERT);
                    }
                    break;
                }
                case 1: // new session
                case 2: {// new session
                    //logPrintf("New Session %d\n", request_index);
                    Handle handle;
                    if (R_FAILED(svcAcceptSession(&handle, hndList[request_index]))) {
                        svcBreak(USERBREAK_ASSERT);
                    }
                    //logPrintf("New Session accepted %x on index %d\n", handle, nmbActiveHandles);
                    if (nmbActiveHandles < MAX_SESSIONS+SERVICE_ENDPOINTS) {
                        hndList[nmbActiveHandles] = handle;
                        nmbActiveHandles++;
                    } else {
                        svcCloseHandle(handle);
                    }
                    break;
                }
                default: { // session
                    //logPrintf("cmd handle %x\n", request_index);
                //	__asm("bkpt #0");
                    handle_commands(&thread_data);
                    reply_target = hndList[request_index];
                    break;
                }
            }
        }
    } while (!term_request);

    srvUnregisterService("nfc:m");
    srvUnregisterService("nfc:u");

    svcCloseHandle(*hndNfuM);
    svcCloseHandle(*hndNfuU);
    svcCloseHandle(*hndNotification);

    return 0;
}
