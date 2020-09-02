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
        static void DisplayError(const char *str);
        static u32 PromptForNewCombo(u32 combo);
        void ShowWarning();
        DirectoryLister *GetDirectory(){
            return &m_directory;
        }
        AmiiboFile *GetAmiibo(){
            return &m_amiibo;
        }
        int GetTagState(bool skip = true){
            return m_state.Get(skip);
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
        void UpdateLastCommandTime(u64 time){
            m_lastcommandtime = time;
        }
        u64 GetLastCommandTime(){
            return m_lastcommandtime;
        }
        u32 GetMenuCombo(){
            return m_menucombo;
        }
        void SetMenuCombo(u32 combo){
            m_menucombo = combo;
        }
        int m_selected = 0;

    private:
        MyThread m_hidthread;
        MyThread m_eventthread;
        uint8_t m_hidthreadcreated = 0;
        DirectoryLister m_directory;
        AmiiboFile m_amiibo;
        TagState m_state;
        Handle m_taginrange;
        Handle m_tagoutofrange;
        u64 m_lastcommandtime;
        u32 m_menucombo = KEY_START | KEY_L | KEY_DDOWN;
};