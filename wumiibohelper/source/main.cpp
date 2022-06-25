#include <iostream>
#include <3ds.h>
#include "ui.hpp"
#include "app.hpp"
#define freeze while(aptMainLoop()) {};
uiFuncs_s ui;
int main()
{
    romfsInit();
    acInit();
    amInit();
    cfguInit();
    //ui.debug = true;
    gdbHioDevInit();
    gdbHioDevRedirectStdStreams(true, true, true);
	APT_SetAppCpuTimeLimit(30);
	aptSetSleepAllowed(false);
	aptSetHomeAllowed(false);
	
    ui.done = false;
	threadCreate((ThreadFunc)&uiThread, nullptr, 0x1000, 0x28, 1, true);
    App app;
    app.DoStuffBeforeMain();
    app.MainLoop();
    ui.done = true;
    svcSleepThread(1e+9);
    socExit();
    cfguExit();
    amExit();
    acExit();
    return 0;
}