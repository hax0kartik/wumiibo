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
    /*
    Utils util;
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);
    romfsInit();
    amInit();
    acInit();
    u32 status = 0;
    ACU_GetStatus(&status);
    std::cout << "Status: " << status << "\n";
    if(status == 3)
    {
        std::cout << "[*] Fetching latest Json files from Github.\n";
       // util.DownloadGamesIDJson();
       // util.DownloadAmiibosJson();
        std::cout << "[+] Downloaded.\n";
    }
    else
    {
        std::cout << "[*] Internet is not connected. Reading in-built json files.\n";
        util.ReadGamesIDJson("romfs:/gameids.json");
        util.ReadAmiibosJson("romfs:/amiibos.json");
        std::cout << "[+] Read\n";
    }

    std::cout << "[*] Getting list of installed titles.\n";
    Result ret = 0;
    u32 count = 0;
    u32 readcount = 0;
    ret = AM_GetTitleCount(MEDIATYPE_SD, &count);
    if(ret > 0)
    {
        std::cout << "[-] AM_GetTitleCount failed.\n";
        freeze;
    }
    std::cout << "installed titles: " << count << "\n";
    u64 *tids = new u64[count];
    ret = AM_GetTitleList(&readcount, MEDIATYPE_SD, count, tids);
    if(ret > 0)
    {
        std::cout << "[-] AM_GetTitleList failed.\n";
        freeze;
    }

    std::cout << "[+] Fetched.\n";
    std::cout << "[*] Generating amiibos for compatible titles.\n";
    util.PopulateAmiiboMap(tids, readcount);
    util.GenerateAmiibos();
    std::cout << "[+] Generated.\n";
    freeze;
    */
    amExit();
    acExit();
    return 0;
}