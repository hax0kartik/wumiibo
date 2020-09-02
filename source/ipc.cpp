#include "ipc.h"
#include "bswap.h"
#include "mythread.h"
extern "C"{
    #include "logger.h"
}
#include <cstdio>
#include <cstring>

extern u8 statbuf[0x800];


char* GetCommandName(u16 cmdid)
{
    static char *cmdstr;

    switch (cmdid) {
		case 0x0001: cmdstr = "Initialize"; break;
		case 0x0002: cmdstr = "Shutdown"; break;
		case 0x0003: cmdstr = "StartCommunication"; break;
		case 0x0004: cmdstr = "StopCommunication"; break;
		case 0x0005: cmdstr = "StartTagScanning"; break;
		case 0x0006: cmdstr = "StopTagScanning"; break;
		case 0x0007: cmdstr = "LoadAmiiboData"; break;
		case 0x0008: cmdstr = "ResetTagScanState"; break;
		case 0x0009: cmdstr = "UpdateStoredAmiiboData"; break;
		case 0x000B: cmdstr = "GetTagInRangeEvent"; break;
		case 0x000C: cmdstr = "GetTagOutOfRangeEvent"; break;
		case 0x000D: cmdstr = "GetTagState"; break;
		case 0x000F: cmdstr = "CommunicationGetStatus"; break;
		case 0x0010: cmdstr = "GetTagInfo2"; break;
		case 0x0011: cmdstr = "GetTagInfo"; break;
		case 0x0012: cmdstr = "CommunicationGetResult"; break;
		case 0x0013: cmdstr = "OpenAppData"; break;
		case 0x0014: cmdstr = "InitializeWriteAppData"; break;
		case 0x0015: cmdstr = "ReadAppData"; break;
		case 0x0016: cmdstr = "WriteAppData"; break;
		case 0x0017: cmdstr = "GetAmiiboSettings"; break;
		case 0x0018: cmdstr = "GetAmiiboConfig"; break;
		case 0x0019: cmdstr = "GetAppDataInitStruct"; break;
        case 0x001A: cmdstr = "MountRomData"; break;
        case 0x001B: cmdstr = "GetAmiiboIdentificationBlock"; break;
		case 0x001F: cmdstr = "StartOtherTagScanning"; break;
		case 0x0020: cmdstr = "SendTagCommand"; break;
		case 0x0021: cmdstr = "Cmd21"; break;
		case 0x0022: cmdstr = "Cmd22"; break;
        case 0x401: cmdstr = "Reset"; break;
        case 0x402: cmdstr = "GetAppDataConfig"; break;
        case 0x407: cmdstr = "IsAppdatInited called"; break; 
		case 0x404: cmdstr = "SetAmiiboSettings"; break;
        default:
            cmdstr = "Unknown Command called"; break;
	}
    return cmdstr;
}

void IPC::Debug(u32 *cmdbuf, char *str)
{
    //logStr("Commmandddd");
    printf("%s : %s\n", str, GetCommandName(cmdbuf[0] >> 16));
    u32 normal = cmdbuf[0] >> 6 & 0x3F;
    u32 translate = cmdbuf[0] & 0x3F;
    for(int i = 0; i < normal; i++)
        printf("cmdbuf[%d]:%X\n", i + 1, cmdbuf[i + 1]);
}

int ShowResult(Result ret)
{
    svcKernelSetState(0x10000, 2 | 1);
    Draw_SetupFramebuffer();
	Draw_Lock();
	Draw_ClearFramebuffer();
	Draw_FlushFramebuffer();
    Draw_DrawFormattedString(15, 10, COLOR_WHITE, "Result %08X", ret);
    u32 key = waitInput();
    Draw_RestoreFramebuffer();
	Draw_Unlock();
	svcKernelSetState(0x10000, 2 | 1);
    return 0;
}

int ShowString(char *str)
{
    svcKernelSetState(0x10000, 2 | 1);
    Draw_SetupFramebuffer();
	Draw_Lock();
	Draw_ClearFramebuffer();
	Draw_FlushFramebuffer();
    Draw_DrawFormattedString(15, 10, COLOR_WHITE, str);
    u32 key = waitInput();
    Draw_RestoreFramebuffer();
	Draw_Unlock();
	svcKernelSetState(0x10000, 2 | 1);
    return 0;
}

void IPC::HandleCommands(NFC* nfc)
{
    u32 *cmdbuf = getThreadCommandBuffer();
    u16 cmdid = cmdbuf[0] >> 16;
    Debug(cmdbuf, "Recieved");

    if(cmdid != 0xF && cmdid != 0xD && m_hasCalled0xC)
    {
        printf("Updating Last called Command Time\n");
        nfc->UpdateLastCommandTime(osGetTime()); 
    }
    switch(cmdid)
    {
        case 1: // Initalize
        {
            nfc->SetTagState(TagStates::ScanningStopped);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 2: // Finalize
        {
            nfc->SetTagState(TagStates::Uninitialized);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 3: // StartCommunication
        {
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 4: // StopCommunication
        {
            cmdbuf[1] = 0;
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            break;
        }

        case 5: // StartTagScanning
        {
            cmdbuf[1] = 0;
            nfc->SetTagState(TagStates::Scanning);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            break;
        }

        case 6: // StopTagScanning
        {
            nfc->SetTagState(TagStates::ScanningStopped);
            cmdbuf[1] = 0;
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            break;
        }

        case 7: // LoadAmiiboData
        {
            // return 0xC8C1760A if data is invalid
            nfc->SetTagState(TagStates::DataReady);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 8: // ResetTagState
        {
            nfc->SetTagState(TagStates::OutOfRange);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 9: // UpdateStoredAmiiboData
        {
            char *str = nfc->GetDirectory()->GetSelectedFileLocation();
            nfc->GetAmiibo()->SaveDecryptedFile();
            Result ret = nfc->GetAmiibo()->WriteDecryptedFile(str);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0xB: // GetTagInRange
        {
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 2);
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            cmdbuf[3] = (u32)*nfc->GetInRangeEvent();
            break;
        }

        case 0xC: // GetTagOutOfRange
        {   
            m_hasCalled0xC = 1; 
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 2);
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            cmdbuf[3] = (u32)*nfc->GetOutOfRangeEvent();
            break;
        }

        case 0xD: // GetTagState
        {
            cmdbuf[0] = IPC_MakeHeader(cmdid, 2, 0);
            cmdbuf[1] = 0;
            cmdbuf[2] = nfc->GetTagState();
            if(nfc->GetTagState() == TagStates::Scanning && nfc->GetAmiibo()->HasParsed())
                nfc->SetTagState(TagStates::InRange);
            break;
        }


        case 0xF: // CommunicationGetStatus
        {
            cmdbuf[0] = IPC_MakeHeader(cmdid, 2, 0);
            cmdbuf[1] = 0;
            cmdbuf[2] = 2; // Communication Established Successfully
            break;
        }

        case 0x11: // GetTagInfo
        {
            Amiibo_TagInfo *tag = nfc->GetAmiibo()->GetTagInfo();
            tag->id_offset_size = 7;
            tag->unk_x2 = 0;
            tag->unk_x3 = 2;
            memcpy(&cmdbuf[2], tag, 0x2C);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 12, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x12: // CommunicationGetResult
        {
            cmdbuf[0] = IPC_MakeHeader(0x12, 2, 0);
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            break;
        }

        case 0x13: // OpenAppData
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            uint32_t appid = bswap_32(cmdbuf[1]);
            
            if(plaindata->flag << 26 >> 31)
            {
               if(appid == plaindata->appDataConfig.appid)
                  cmdbuf[1] = 0;
               else
               {
                  cmdbuf[1] = 0xC8A17638; // AppId incorrect
               }				
            }
            else
            {
               cmdbuf[1] = 0xC8A17620; // Not Initialized
            }
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            break;
        }

        case 0x14: // InitializeAppData
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            uint32_t appid = cmdbuf[1];
            uint32_t size = cmdbuf[2];
            uint32_t bufptr = cmdbuf[0x12];
            FS_ProgramInfo info;
            FSUSER_GetProgramLaunchInfo(&info, cmdbuf[16]);
            plaindata->appDataConfig.titleid = bswap_64(info.programId);
            plaindata->appDataConfig.appid = bswap_32(appid);
            plaindata->flag |= 0x20u;
            memcpy(&plaindata->AppData[0], (void*)bufptr, size);
            char *str = nfc->GetDirectory()->GetSelectedFileLocation();
            nfc->GetAmiibo()->SaveDecryptedFile();
            Result ret = nfc->GetAmiibo()->WriteDecryptedFile(str);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x15: // GetAppData
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 2);
            cmdbuf[1] = 0;
            cmdbuf[2] = IPC_Desc_StaticBuffer(0xD8, 0);
            memset(&statbuf, 0, 0xD8);
            memcpy(&statbuf, &plaindata->AppData[0], 0xD8);
            cmdbuf[3] = (u32)&statbuf;
            break;
        }

        case 0x16: // WriteAppData
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            memcpy(&plaindata->AppData[0], (void*)cmdbuf[11], 0xD8);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            char *str = nfc->GetDirectory()->GetSelectedFileLocation();
            nfc->GetAmiibo()->SaveDecryptedFile();
            Result ret = nfc->GetAmiibo()->WriteDecryptedFile(str);
            break;
        }

        case 0x17: // GetAmiiboSettings
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            if(!(plaindata->flag & 0x10))
            {
                memset(&plaindata->settings, 0, 0xA8);
                //printf("0x17 UNINTIALIZED");
                cmdbuf[1]= 0xC8A17628;
            }
            else
            {
                cmdbuf[1] = 0;
            }

            cmdbuf[0] = IPC_MakeHeader(cmdid, 0x2B, 0);
            memcpy(&cmdbuf[2], &plaindata->settings, 0xA8);
            break;
        }

        case 0x18: // GetAmiiboConfig
        {
            Amiibo_AmiiboConfig config;
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            Amiibo_IdentificationBlock *identityblock = nfc->GetAmiibo()->GetIdentity();
            config.lastwritedate.year = plaindata->lastwritedate.year;
            config.lastwritedate.month = plaindata->lastwritedate.month;
            config.lastwritedate.day = plaindata->lastwritedate.day;
            config.write_counter = plaindata->writecounter;
            config.characterID[0] = identityblock->id[0];
            config.characterID[1] = identityblock->id[1];
            config.characterID[2] = identityblock->char_variant;
            config.series = identityblock->series;
            config.amiiboID[0] = identityblock->model_no[0];
            config.amiiboID[1] = identityblock->model_no[1];
            config.type = identityblock->figure_type;
            config.pagex4_byte3 = plaindata->pagex4_byte3;
            config.appdata_size = 0xD8;
            memcpy(&cmdbuf[2], &config, 0x10);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 17, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x19: //GetAppDataInitStruct 
        {
            cmdbuf[0] = IPC_MakeHeader(cmdid, 16, 0);
            cmdbuf[1] = 0;
            memset(&cmdbuf[2], 0, 0x3C);
            break;
        }

        case 0x1A: // 
        {
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x1B: // GetAmiiboIdentificationBlock
        {
            Amiibo_IdentificationBlock *identityblock = nfc->GetAmiibo()->GetIdentity();
            memcpy(&cmdbuf[2], identityblock, 0x36);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 15, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x402: // GetAppDataConfig
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            plaindata->appDataConfig.unk2 = 2;
            plaindata->appDataConfig.tid_related = -1;
            if(plaindata->flag << 26 >> 31)
            {
               switch(plaindata->appDataConfig.titleid >> 28)
               {
                  case 0:
                  case 2:
                    plaindata->appDataConfig.tid_related = 0;
                    break;
                  case 1:
                    plaindata->appDataConfig.tid_related = 1;
               }
            }
            memcpy((uint8_t*)&cmdbuf[2], (uint8_t*)&plaindata->appDataConfig, sizeof(plaindata->appDataConfig));
            cmdbuf[0] = IPC_MakeHeader(cmdid, 17, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x404: // SetAmiiboSettings
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            memcpy(&plaindata->settings, &cmdbuf[1], 0xA4);
            if(!(plaindata->flag << 27 >> 31) & 0xF)
            {
                // TODO Set Country code and date correctly
                Date date(28, 8, 20);
                u16 raw = date.getraw();
                plaindata->settings.setupdate = date;
            }
            plaindata->flag = ((plaindata->flag & 0xF0) | (plaindata->settings.flags & 0xF) | 0x10);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 1, 0);
            cmdbuf[1] = 0;
            break;
        }

        case 0x407: // IsAppDataSet
        {
            Amiibo_PlainData *plaindata = nfc->GetAmiibo()->GetPlainData();
            uint32_t isSet = (plaindata->flag << 26 >> 31);
            printf("IsSet %d\n", isSet);
            cmdbuf[0] = IPC_MakeHeader(cmdid, 2, 0);
            cmdbuf[1] = 0;
            cmdbuf[2] = isSet;
            break;
        }

        default:
            printf("Unimplemented Command %08X\n", cmdbuf[0]);
    }
    Debug(cmdbuf, "Sent");
}