#pragma once
#include <3ds.h>
#include <functional>
extern "C"
{
    #include "input.h"
    #include "draw.h"
    #include "mythread.h"
}
class Menu
{
    public:
        void CreateHidThread();
        void AddCallback(int i, std::function<void(void*)>cb, void *args)
        {
            m_callbacks[i] = cb;
            m_callbacksargs[i] = args;
        }
        static void draw(Menu *menu);

    private:
        MyThread m_hidthread;
        int m_selected = 0;
        uint8_t m_hidthreadcreated = 0;
        std::function<void(void *)>m_callbacks[3];
        void *m_callbacksargs[3];
};