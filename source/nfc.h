#pragma once
#include <3ds.h>
#include "DirectoryLister.h"
#include "AmiiboFile.h"
#include "TagState.h"
extern "C"
{
    #include "input.h"
    #include "draw.h"
    #include "mythread.h"
}
class NFC
{
    public:
        void CreateHidThread();
        static void DrawMenu(NFC *nfc);
        DirectoryLister *GetDirectory(){
            return &m_directory;
        }
        AmiiboFile *GetAmiibo(){
            return &m_amiibo;
        }
        int GetTagState(){
            return m_state.Get();
        }
        void SetTagState(int state){
            m_state = state;
        }
        Handle *GetInRangeEvent(){
            return &m_taginrange;
        }
        Handle *GetOutOfRangeEvent(){
            return &m_tagoutofrange;
        }
        LightEvent *GetDoEvents(){
            return &m_doevents;
        }
        int m_selected = 0;

    private:
        MyThread m_hidthread;
        uint8_t m_hidthreadcreated = 0;
        DirectoryLister m_directory;
        AmiiboFile m_amiibo;
        TagState m_state;
        Handle m_taginrange;
        Handle m_tagoutofrange;
        LightEvent m_doevents;
};