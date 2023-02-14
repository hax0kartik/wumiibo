#pragma once 
#include <string>
#include <cstdio>
#include <3ds.h>
#include <sys/stat.h>

namespace Utils{
    namespace Misc{
        static inline void RebootToSelf(){
            nsInit();
            Result ret = NS_RebootToTitle(MEDIATYPE_SD, 0x0004000000DF1100);
            if(R_FAILED(ret))
                *(u32*)ret = 0x123; // Shouldn't have happened
        }

        static inline void Reboot(){
            nsInit();
            Result ret = NS_RebootSystem();
            if(R_FAILED(ret))
                *(u32*)ret = 0x122; // Shouldn't have happened
        }

        static inline bool IsReboot(){
            uint8_t *firmparams = new uint8_t[0x1000];
            Result ret = pmAppInit();
            if(R_SUCCEEDED(ret)){
                ret = PMAPP_GetFIRMLaunchParams(firmparams, 0x1000);
                if(R_SUCCEEDED(ret)){
                    u64 tid = 0x0004000000DF1100;
                    return memcmp(firmparams + 0x440, &tid, sizeof(tid)) == 0;
                }
            }
            pmAppExit();
            delete[] firmparams;
            return false;
        }

        static inline bool CheckWumiibo(){
            if(R_SUCCEEDED(nfcInit(NFC_OpType_NFCTag))){
                Handle nfchandle = nfcGetSessionHandle();
                /* The below is a special IPC command which is only implemented in wumiibo */
                u32 *cmdbuf = getThreadCommandBuffer();
                cmdbuf[0] = IPC_MakeHeader(0x24, 0, 0);
                if(R_SUCCEEDED(svcSendSyncRequest(nfchandle))){
                    Result ret = cmdbuf[1];
                    nfcExit();
                    return ret != 0xD900182F;
                }
                nfcExit();
            }
        return false;
        }

        static inline bool SetWumiiboState(bool newstate){
            if(newstate == false){ // disable wumiibo 
                if(rename("/luma/titles/0004013000004002", "/luma/titles/wumiibo") != 0)
                    return false;
            }
            else{
                if(rename("/luma/titles/wumiibo", "/luma/titles/0004013000004002") != 0)
                    return false;
            }
            return true;
        }

        static inline void CreateBin(std::string name, std::string amiiboid, std::string loc, std::string tid){
            std::string filename = loc;
            mkdir(filename.c_str(), 0777);
            filename = filename + "/" + tid;
            mkdir(filename.c_str(), 0777);
            filename = loc + "/" + tid + "/" + name + ".bin";
            uint64_t amid = strtoll(amiiboid.c_str(), NULL, 16);
            FILE *file = fopen(filename.c_str(), "r");
            /* same name file exists */
            if(file){
                fclose(file);
                for(int i = 1; ;i++) {
                    filename = loc + "/" + tid + "/" + name + "_" + std::to_string(i) + ".bin";
                    file = fopen(filename.c_str(), "r");
                    if(!file) break;
                    fclose(file);
                }
            }
            file = fopen(filename.c_str(), "wb");
            if(!file)
                return;
            char *data = new char[540];
            memset(data, 0, 540);
            data[0x2] = 0x0F;
            data[0x3] = 0xE0;
            uint8_t *amiddata = (uint8_t*)&amid;
            int offset = 0x1DC;
            for(int i = 7; i >= 0; i--){
                data[offset++] = amiddata[i];
            }
            fwrite(data, 540, 1, file);
            fclose(file);
            delete[] data;
        }

        static inline void EnableGamePatching(){
            std::string data;
            FILE *f = fopen("/luma/config.ini", "r+");
            if(f)
            {
                fseek(f, 0L, SEEK_END);
                size_t size = ftell(f);
                data.resize(size);
                fseek(f, 0L, SEEK_SET);
                fread(&data[0], size, 1, f);
                const std::string s = "enable_game_patching = 1";
                const std::string s2 = "enable_external_firm_and_modules = 1";
                auto found = data.find("enable_game_patching");
                if(found != std::string::npos){
                    data.replace(found, s.length(), s);
                }
                auto found2 = data.find("enable_external_firm_and_modules");
                if(found2 != std::string::npos){
                    data.replace(found2, s2.length(), s2);
                }
                fseek(f, 0L, SEEK_SET);
                fwrite(&data[0], data.length(), 1, f);
                fclose(f);
            }
        }
    }
}