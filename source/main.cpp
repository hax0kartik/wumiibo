#include <3ds.h>
#include "ipc.h"
#include "nfc.h"
extern "C"
{
    #include "services.h"
    #include "mythread.h"
    #include "logger.h"
    #include "csvc.h"
}
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

extern "C"
{
    extern u32 __ctru_heap, __ctru_heap_size, __ctru_linear_heap, __ctru_linear_heap_size;
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    // this is called before main
    void __system_allocateHeaps(void)
    {
        u32 tmp=0;
	    __ctru_heap_size = 0x8000;
	    // Allocate the application heap
	    __ctru_heap = 0x08000000;
	    svcControlMemoryEx(&tmp, __ctru_heap, 0x0, __ctru_heap_size, MEMOP_ALLOC, (MemPerm)(MEMPERM_READWRITE | MEMREGION_BASE), false);
	    // Set up newlib heap
	    fake_heap_start = (char*)__ctru_heap;
	    fake_heap_end = fake_heap_start + __ctru_heap_size;
    }

    void __appInit() {
        srvSysInit();
        if(cfguInit() > 0)		
            CRASH;

        mappableInit(0x10000000, 0x14000000);

        if(hidInit() != 0)
            CRASH;

       // gdbHioDevInit();
       // gdbHioDevRedirectStdStreams(false, true, false);
        fsSysInit();
        psInit();
      //  logInit();
    }

    // this is called after main exits
    void __appExit() {
       // logExit();
        psExit();
        fsExit();
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
        __system_allocateHeaps();
        __appInit();
    }

    void __ctru_exit(int rc) {
        __appExit();
        __sync_fini();
        __libc_fini_array();
        svcExitProcess();
    }
}

u8 ALIGN(8) statbuf[0x800] = {0};
int main() {

    int nmbActiveHandles;
    IPC ipc;
    NFC nfc;
    nfc.ReadConfiguration();
    nfc.CreateHidThread();

    Handle *hndNfuU;
    Handle *hndNfuM;
    Handle *hndNotification;
    Handle hndList[MAX_SESSIONS+SERVICE_ENDPOINTS];

    hndNotification = &hndList[0];
    hndNfuU = &hndList[1];
    hndNfuM = &hndList[2];
    nmbActiveHandles = SERVICE_ENDPOINTS;

    u32* staticbuf = getThreadStaticBuffers();
    staticbuf[0]  = IPC_Desc_StaticBuffer(0x800, 0);
    staticbuf[1]  = (u32)&statbuf;
    staticbuf[2]  = IPC_Desc_StaticBuffer(0x800, 0);
    staticbuf[3]  = (u32)&statbuf;
    staticbuf[4]  = IPC_Desc_StaticBuffer(0x800, 0);
    staticbuf[5]  = (u32)&statbuf;


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
                    ipc.HandleCommands(&nfc);
                    reply_target = hndList[request_index];
                    break;
                }
            }
        }
    } while (!term_request);

    nfc.FreeUpThreads();
    srvUnregisterService("nfc:m");
    srvUnregisterService("nfc:u");

    svcCloseHandle(*hndNfuM);
    svcCloseHandle(*hndNfuU);
    svcCloseHandle(*hndNotification);

    return 0;
}