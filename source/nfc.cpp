#include "nfc.h"
#include <stdio.h>
static u8 ALIGN(8) hidThreadStack[0x1000];

void hidThread(void *arg)
{
    NFC *nfc = (NFC*)arg;
    while(1)
    {
        svcSleepThread(0.1e+9);
        hidScanInput();
        if(hidKeysDown() & KEY_START)
        {
            //printf("KEY_START pressed\n");
            nfc->DrawMenu(nfc);
            if(nfc->m_selected == 0)
            {
                nfc->GetDirectory()->PopulateEntries("/wumiibo");
                nfc->GetDirectory()->ListEntries();
                nfc->GetDirectory()->ConstructFileLocation();
            }
            else
            {
                nfc->GetDirectory()->Reset();
                nfc->SetTagState(TagStates::OutOfRange);
                svcSignalEvent(*nfc->GetOutOfRangeEvent());
            }
            
        }
    }
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
        LightEvent_Init(&m_doevents, RESET_ONESHOT);
        svcCreateEvent(&m_taginrange, RESET_ONESHOT);
        svcCreateEvent(&m_tagoutofrange, RESET_ONESHOT);
        // m_selected = 1;
        MyThread_Create(&m_hidthread, hidThread, this, hidThreadStack, 0x1000, 53, -2);
        m_hidthreadcreated = 1;
    }
}