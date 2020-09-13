// License for this file: ctrulib's license
// Copyright AuroraWright, TuxSH 2019-2020

#pragma once

#include <3ds/services/pmapp.h>
#include <3ds/services/pmdbg.h>
Result PMDBG_GetCurrentAppInfo(FS_ProgramInfo *outProgramInfo, u32 *outPid, u32 *outLaunchFlags);