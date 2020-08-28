#pragma once
#include <3ds.h>
#include "DirectoryLister.h"
#include "AmiiboFile.h"
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
        int m_selected = 0;

    private:
        MyThread m_hidthread;
        uint8_t m_hidthreadcreated = 0;
        DirectoryLister m_directory;
        AmiiboFile m_amiibo;
};