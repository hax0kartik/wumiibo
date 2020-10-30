
#pragma once
#include <3ds.h>

typedef struct __attribute__((packed, aligned(4)))
{
    char magic[4];
    u16 formatVersionMajor, formatVersionMinor;

    u32 config, multiConfig, bootConfig;
    u64 hbldr3dsxTitleId;
    u32 rosalinaMenuCombo;
} CfgData;


class LumaConfig{
    public:
        void ReadConfig();
        void EnableGamePatching();
        void WriteConfig();
    private:
        CfgData m_config;
};