#include "ui.hpp"

void uiThread(void *arg)
{
    gfxInitDefault();
    if(ui.debug) consoleInit(GFX_BOTTOM, NULL);
    
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create screen
	ui.top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    if(ui.debug == false) ui.bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    ui.bot_text_buf = C2D_TextBufNew(4096);
    ui.top_text_buf = C2D_TextBufNew(4096);

    ui.font = C2D_FontLoad("romfs:/profont.bcfnt");

    printf("GUI thread created");
    while(aptMainLoop())
    {
        if(ui.done) break;
      //  svcWaitSynchronization(ui.event, U64_MAX);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		
        if(ui.debug == false && ui.bottom != nullptr)
        {
            C2D_TargetClear(ui.bottom, C2D_Color32(0xec, 0xf0, 0xf1, 0xFF));
            C2D_SceneBegin(ui.bottom);
            if(ui.bot_func) ui.bot_func();
        }

        if(ui.top != nullptr)
        {
            C2D_TargetClear(ui.top, C2D_Color32(0xec, 0xF0, 0xf1, 0xFF));
            C2D_SceneBegin(ui.top);
            if(ui.top_func) ui.top_func();
        }
        C3D_FrameEnd(0);
        
    }
    romfsExit();
    gfxExit();
    threadExit(0);
}