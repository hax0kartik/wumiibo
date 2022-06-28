#include <cstdio>
#include <malloc.h>
#include "app.hpp"

void App::Intialize(){
    gfxInitDefault();
    romfsInit();
    amInit();
    acInit();
    gdbHioDevInit();
    gdbHioDevRedirectStdStreams(true, true, true);
    uint32_t status = 0;
    ACU_GetStatus(&status);
    if(status == 3){
        uint32_t *socbuf = (uint32_t*)memalign(0x1000, 0x100000);
        if(R_SUCCEEDED(socInit(socbuf, 0x100000))){
            m_connected = true;
            m_downloadmanager.Intialize();
        };
    }
    ui::Intialize();
    ChangeState(ui::Initial);
}

void App::RunLoop(){
    while(aptMainLoop()){
        hidScanInput();
        if(hidKeysDown() & KEY_START)
            break;
        auto newstate = m_currstate->HandleEvent();
        if(newstate.has_value()){
            ChangeState(newstate.value());
        }
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        m_currstate->RenderLoop();
        C3D_FrameEnd(0);
    }
    /* ChangeState() couldn't have been called already so we are safe */
    if(m_currstate) m_currstate->OnStateExit(this);
}

void App::ChangeState(auto newstate){
    if(m_currstate) m_currstate->OnStateExit(this);
    switch(newstate){
        case ui::Initial:
            m_currstate = &m_initial;
            break;
        case ui::MainMenu:
            m_currstate = &m_mainmenu;
            break;
        case ui::Download:
            m_currstate = &m_download;
            break;
        case ui::GameSelection:
            m_currstate = &m_gameselection;
            break;
        case ui::AmiiboSelection:
            m_currstate = &m_amiiboselection;
            break;
        case ui::ToggleState:
            m_currstate = &m_toggle;
            break;
    }
    m_currstate->OnStateEnter(this);
}