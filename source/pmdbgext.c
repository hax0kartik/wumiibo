// License for this file: ctrulib's license
// Copyright AuroraWright, TuxSH 2019-2020

#include <string.h>
#include <3ds/types.h>
#include <3ds/result.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/services/pmdbg.h>
#include <3ds/ipc.h>

Result PMDBG_GetCurrentAppInfo(FS_ProgramInfo *outProgramInfo, u32 *outPid, u32 *outLaunchFlags)
{
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x100, 0, 0);
    if(R_FAILED(ret = svcSendSyncRequest(*pmDbgGetSessionHandle()))) return ret;

    memcpy(outProgramInfo, cmdbuf + 2, sizeof(FS_ProgramInfo));
    *outPid = cmdbuf[6];
    *outLaunchFlags = cmdbuf[7];
    return cmdbuf[1];
}