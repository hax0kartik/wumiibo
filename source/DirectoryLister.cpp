#include "DirectoryLister.h"
#include <cstring>
#include <cstdlib>
#include <string>
extern "C"
{
    #include "input.h"
    #include "draw.h"
}

Result DirectoryLister::PopulateEntries(char *directory)
{
    memset(m_filename, 0, 50);
    strcpy(m_filename, directory);
    Result ret = 0;
    ret = FSUSER_OpenArchive(&m_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, NULL));
    if(ret) return ret;
    ret = FSUSER_OpenDirectory(&m_fshandle, m_archive, fsMakePath(PATH_ASCII, directory));
    if(ret) return ret;
    ret = FSDIR_Read(m_fshandle, (uint32_t*)&m_readentries, 10, m_entries);
    return ret;
}

Result DirectoryLister::ListEntries()
{
    svcKernelSetState(0x10000, 2|1);

	Draw_SetupFramebuffer();
	Draw_Lock();
	Draw_ClearFramebuffer();
	Draw_FlushFramebuffer();
    m_selected = 0;
    while(true)
    {
        for(int i = 0; i < m_readentries; i++)
        {
            if(i == m_selected) 
                Draw_DrawCharacter(5, 10 + 10 * i, COLOR_TITLE, '>');
            else
                Draw_DrawCharacter(5, 10 + 10 * i, COLOR_TITLE, ' ');

	        Draw_DrawString16(15, 10 + 10 * i, COLOR_WHITE, m_entries[i].name);
        }
	    u32 key = waitInput();
        if(key & BUTTON_DOWN)
        {
            m_selected++;
            if (m_selected > m_readentries - 1) m_selected = 0;
        }
        
        if(key & BUTTON_UP)
        {
            m_selected--;
            if(m_selected < 0) m_selected = m_readentries - 1;
        }

        if(key & BUTTON_A)
        {
            break;
        }

        if(key & BUTTON_B)
        {
            m_selected = -1;
            break;
        }

    }
	Draw_RestoreFramebuffer();
	Draw_Unlock();
	svcKernelSetState(0x10000, 2|1);
    return m_selected;
}

void DirectoryLister::ConstructFileLocation()
{
    uint16_t *name = m_entries[m_selected].name;
    uint8_t u8name[50];
    memset(u8name, 0, 50);
    int len = utf16_to_utf8(u8name, name, 50);
    u8name[len] = 0;
    strcat(m_filename, "/");
    strcat(m_filename, (const char*)u8name);
}