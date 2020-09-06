#include "Configuration.h"
#include <cstdlib>
extern "C"
{
    #include "ini.h"
    #include "logger.h"
}

static int keystrtokeyval(char *str)
{
    int val = 0;
    key_s keys[] = {
        { "A",      KEY_A},
        { "B",      KEY_B},
        { "SELECT", KEY_SELECT},
        { "START" , KEY_START},
        { "RIGHT",  KEY_DRIGHT},
        { "LEFT",   KEY_DLEFT},
        { "UP",     KEY_DUP},
        { "DOWN",   KEY_DDOWN},
        { "R",      KEY_R},
        { "L",      KEY_L},
        { "X",      KEY_X},
        { "Y",      KEY_Y}

    };
    char *key;
    key = strtok(str, "+");

    while(key != NULL)
    {
        for (int i = 0; i < 12; i++)
        {
            if(strcmp(keys[i].key, key) == 0)
                val |= keys[i].val;
        }
        key = strtok(NULL, "+");
    }
    return val;
}

Result Configuration::ReadINI(const char *loc)
{
   Handle fshandle;
   Result ret = FSUSER_OpenFileDirectly(&fshandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, NULL), fsMakePath(PATH_ASCII, "/settings.ini"), FS_OPEN_READ, 0);
   if(ret) return ret;
   u64 size;
   ret = FSFILE_GetSize(fshandle, &size);
   m_data = (char*)malloc(size + 1);
   memset(m_data, 0, size);
   if(!m_data) return -1;
   ret = FSFILE_Read(fshandle, NULL, 0, m_data, size);
   if(ret) return ret;
   ret = FSFILE_Close(fshandle);
   if(ret) return ret;
   return 0;
}

int Configuration::ParseINI()
{
    ini_t config;
    int res = ini_load_string(&config, m_data);
    if(res < 0)
        return -1;
    char *menubuttons = (char*)ini_get(&config, "config", "menubuttons");
    if(!menubuttons) return -2;
    char *debug = (char*)ini_get(&config, "config", "debug");
    if(!debug) return -3;
    m_debug = atoi(debug);
    setLogEnable(m_debug > 0);
    m_keyval = keystrtokeyval(menubuttons);
    ini_free(&config);
    return 0;
}