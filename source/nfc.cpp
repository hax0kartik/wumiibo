#include "nfc.h"
#include <cstring>
#include <cstdlib>
extern "C" {
#include "logger.h"
#include "memory.h"
#include "pmdbgext.h"
}

static u8 ALIGN(8) hidThreadStack[0x1000];
static u8 ALIGN(8) threadStack[0x1000];

extern "C"
{
    bool hidShouldUseIrrst(void) {
        return false;
    }
}

static u64 GetCurrentGame()
{
    pmDbgInit();
    FS_ProgramInfo programInfo;
    u64 tid;
    u32 launchFlags;
    u32 pid;
    Result res = PMDBG_GetCurrentAppInfo(&programInfo, &pid, &launchFlags);
    pmDbgExit();
    if (R_FAILED(res))
        return -1;
    return programInfo.programId;
}

void EventThread(void *arg)
{
    NFC *nfc = (NFC*)arg;
    Handle *taginrange = nfc->GetInRangeEvent();
    Handle *tagoutofrange = nfc->GetOutOfRangeEvent();
    uint8_t doonce = 0;
    while(1)
    {
        svcSleepThread(0.1e+9);
        if(!(nfc->GetAmiibo()->HasParsed()))
                continue;
        
        if(nfc->GetSignal() == 0 && (nfc->GetTagState() == TagStates::Scanning || nfc->GetTagState() == TagStates::ScanningStopped))
        {
            svcSignalEvent(*taginrange);
        }

        if((osGetTime() - nfc->GetLastCommandTime()) > 4000)
        {
            nfc->UpdateLastCommandTime(osGetTime());
            svcSignalEvent(*tagoutofrange);
            svcWaitSynchronization(*taginrange, U64_MAX);
        }
    }
    MyThread_Exit();
}

void hidThread(void *arg)
{
    NFC *nfc = (NFC*)arg;
    while(1)
    {
        svcSleepThread(0.1e+9);
        hidScanInput();
        u32 keysheld = hidKeysHeld();
        u32 combo = nfc->GetMenuCombo();
        if((keysheld & combo) == combo) 
        {
            nfc->StartMenu();
            //printf("KEY_START pressed\n");
            nfc->DrawMenu(nfc);
            if(nfc->m_selected == 0)
            {
                char folder[30];
                char stid[16 + 1];
                memset(folder, 0, 30);
                strcpy(folder, "/wumiibo/");
                nfc->GetAmiibo()->Reset();
                u64 tid = GetCurrentGame();
                hexItoa(tid, stid, 16, true);
                stid[17] = 0;
                strcat(folder, stid);
                if(R_FAILED(nfc->GetDirectory()->PopulateEntries(folder)))
                    nfc->GetDirectory()->PopulateEntries("/wumiibo");
                nfc->GetDirectory()->ListEntries();
                nfc->GetDirectory()->ConstructFileLocation();
                char *str = nfc->GetDirectory()->GetSelectedFileLocation();
                Result ret = nfc->GetAmiibo()->ReadDecryptedFile(str);
                ret = nfc->GetAmiibo()->ParseDecryptedFile();
                if(ret == -1)
                    nfc->DisplayError("File is encrypted, decrypt it using amiitool to use it with wumiibo.");
                else if(ret == -2)
                    nfc->DisplayError("File could not be parsed.");
                else if(ret == 1) // A new UID was generated, save this back to file.
                {
                    nfc->GetAmiibo()->SaveDecryptedFile();
                    nfc->GetAmiibo()->WriteDecryptedFile(str);
                }

            }
            else if(nfc->m_selected == 1)
            {
                nfc->GetAmiibo()->Reset();
                nfc->GetDirectory()->Reset();
                nfc->SetTagState(TagStates::OutOfRange);
                svcSignalEvent(*nfc->GetOutOfRangeEvent());
                svcClearEvent(*nfc->GetInRangeEvent());
                logStr("Force stopped called\n");
            }
            else if(nfc->m_selected == 2)
            {
                nfc->GetAmiibo()->GenerateRandomUID();
            }
            else if(nfc->m_selected == 3)
            {
                svcSignalEvent(*nfc->GetInRangeEvent());
            }
            else if(nfc->m_selected == 4)
            {
                svcSignalEvent(*nfc->GetOutOfRangeEvent());
            }
            nfc->FinishMenu();
        }
    }
}

void NFC::DisplayError(const char *str)
{
    Draw_ClearFramebuffer();
    Draw_DrawString(120, 10, COLOR_TITLE, "Wumiibo Menu");
    Draw_DrawString(15, 20, COLOR_WHITE, str);
    u32 key = waitInput();
}

void NFC::StartMenu()
{
    Draw_Lock();
    svcKernelSetState(0x10000, 1 | 2);
    svcSleepThread(5 * 1000 * 100LL);
    Draw_AllocateFramebufferCache(FB_BOTTOM_SIZE);
    Draw_SetupFramebuffer();
    Draw_ClearFramebuffer();
}

void NFC::FinishMenu()
{
    Draw_FlushFramebuffer();
    Draw_RestoreFramebuffer();
    svcKernelSetState(0x10000, 1 | 2);
    svcSleepThread(5 * 1000 * 100LL);
    Draw_FreeFramebufferCache();
    Draw_Unlock();
}

void NFC::DrawMenu(NFC *nfc)
{
    int size = 5;
    Draw_DrawString(120, 10, COLOR_TITLE, "Wumiibo Menu");
    Draw_DrawString(15, 20, COLOR_WHITE, "Select a figure.");
    Draw_DrawString(15, 30, COLOR_WHITE, "Force Stop Emulation.");
    Draw_DrawString(15, 40, COLOR_WHITE, "Randomize UID(Bypass 1 use per day limit).");
    Draw_DrawString(15, 50, COLOR_WHITE, "Signal TagInRangeEvent.");
    Draw_DrawString(15, 60, COLOR_WHITE, "Signal TagOutOfRangeEvent.");
    if(nfc->GetAmiibo()->HasParsed())
    {
        Draw_DrawString(10, 230, COLOR_WHITE, "Currently Emulating:");
        Draw_DrawString(130, 230, COLOR_TITLE, nfc->GetDirectory()->GetSelectedFileLocation());
    }

    nfc->m_selected = 0;
    while(true)
    {
        for(int i = 0; i < size; i++)
        {
            if(i == nfc->m_selected) 
                Draw_DrawCharacter(5, 20 + 10 * i, COLOR_TITLE, '>');
            else
                Draw_DrawCharacter(5, 20 + 10 * i, COLOR_TITLE, ' ');
        }
        Draw_FlushFramebuffer();
        u32 key = waitInput();
        if(key & BUTTON_DOWN)
        {
            nfc->m_selected++;
            if (nfc->m_selected > size - 1) nfc->m_selected = 0;
        }
        
        if(key & BUTTON_UP)
        {
            nfc->m_selected--;
            if(nfc->m_selected < 0) nfc->m_selected = size - 1;
        }

        if(key & BUTTON_A)
        {
            break;
        }

        if(key & BUTTON_B)
        {
            nfc->m_selected = -1;
            break;
        }
    }
}

void NFC::CreateHidThread()
{
    hidSetRepeatParameters(200, 100);
    if(!m_hidthreadcreated)
    {
        svcCreateEvent(&m_taginrange, RESET_ONESHOT);
        svcCreateEvent(&m_tagoutofrange, RESET_ONESHOT);
        // m_selected = 1;
        MyThread_Create(&m_hidthread, hidThread, this, hidThreadStack, 0x1000, 47, -2);
        MyThread_Create(&m_eventthread, EventThread, this, threadStack, 0x1000, 47, -2);
        m_hidthreadcreated = 1;
    }
}

void NFC::FreeUpThreads()
{
    svcCloseHandle(m_taginrange);
    svcCloseHandle(m_tagoutofrange);
    MyThread_Join(&m_hidthread, 1e+9);
    MyThread_Join(&m_hidthread, 1e+9);
}

void NFC::ReadConfiguration()
{
    //char *name = "/settings.ini";
    Result ret = m_config.ReadINI("/wumiibo.ini");
    if(ret != 0) return;
    ret = m_config.ParseINI();
    if(ret != 0) return;
}