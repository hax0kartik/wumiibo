#include "nfc.h"
#include <stdio.h>
#include <cstdlib>
static u8 ALIGN(8) hidThreadStack[0x1000];
static u8 ALIGN(8) threadStack[0x1000];

extern "C"
{
    bool hidShouldUseIrrst(void)
    {
        return false;
    }
}

void hidThread(void *arg)
{
    NFC *nfc = (NFC*)arg;
    while(1)
    {
        svcSleepThread(0.1e+9);
        hidScanInput();
        u32 keysheld = hidKeysHeld();
        if((keysheld & (KEY_L | KEY_DDOWN | KEY_START)) == (KEY_L | KEY_DDOWN | KEY_START)) 
        {
            //printf("KEY_START pressed\n");
            nfc->DrawMenu(nfc);
            if(nfc->m_selected == 0)
            {
                nfc->GetAmiibo()->Reset();
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
            }
            else if(nfc->m_selected == 1)
            {
                nfc->GetAmiibo()->Reset();
                nfc->GetDirectory()->Reset();
                nfc->SetTagState(TagStates::OutOfRange);
                svcSignalEvent(*nfc->GetOutOfRangeEvent());
            }
            
        }
    }
}

void EventThread(void *arg)
{
    NFC *nfc = (NFC*)arg;
    Handle *taginrange = nfc->GetInRangeEvent();
    Handle *tagoutofrange = nfc->GetOutOfRangeEvent();
    while(1)
    {
        svcSleepThread(0.1e+9);
        if(!(nfc->GetAmiibo()->HasParsed()))
            continue;
        svcSignalEvent(*taginrange);
        u64 time = osGetTime();
        if((time - nfc->GetLastCommandTime()) > 3000)
            svcSignalEvent(*tagoutofrange);
    }
    MyThread_Exit();
}

void NFC::DisplayError(const char *str)
{
    svcKernelSetState(0x10000, 2|1);
	Draw_SetupFramebuffer();
	Draw_Lock();
	Draw_ClearFramebuffer();
	Draw_FlushFramebuffer();
    Draw_DrawString(120, 10, COLOR_TITLE, "Wumiibo Menu");
    Draw_DrawString(15, 20, COLOR_WHITE, str);
    u32 key = waitInput();
    Draw_RestoreFramebuffer();
	Draw_Unlock();
	svcKernelSetState(0x10000, 2 | 1);
}

void NFC::DrawMenu(NFC *nfc)
{
    int size = 2;
    svcKernelSetState(0x10000, 2|1);

	Draw_SetupFramebuffer();
	Draw_Lock();
	Draw_ClearFramebuffer();
	Draw_FlushFramebuffer();

    Draw_DrawString(120, 10, COLOR_TITLE, "Wumiibo Menu");
    Draw_DrawString(15, 20, COLOR_WHITE, "Select a figure.");
    Draw_DrawString(15, 30, COLOR_WHITE, "Force Stop Emulation.");
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
	Draw_RestoreFramebuffer();
	Draw_Unlock();
	svcKernelSetState(0x10000, 2 | 1);
}

void NFC::CreateHidThread()
{
    if(!m_hidthreadcreated)
    {
        svcCreateEvent(&m_taginrange, RESET_ONESHOT);
        svcCreateEvent(&m_tagoutofrange, RESET_ONESHOT);
        // m_selected = 1;
        MyThread_Create(&m_hidthread, hidThread, this, hidThreadStack, 0x1000, 15, -2);
        MyThread_Create(&m_eventthread, EventThread, this, threadStack, 0x1000, 15, -2);
        m_hidthreadcreated = 1;
    }
}