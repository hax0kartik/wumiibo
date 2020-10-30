#include "config.hpp"
#include <cstdio>

void LumaConfig::ReadConfig()
{
    FILE *f = fopen("/luma/config.bin", "r");
    if(f)
    {
        fread(&m_config, sizeof(CfgData), 1, f);
        fclose(f);
    }
}

void LumaConfig::EnableGamePatching()
{
    m_config.config |= 1 << 3;
}

void LumaConfig::WriteConfig()
{
    FILE *f = fopen("/luma/config.bin", "wb");
    if(f)
    {
        fwrite(&m_config, sizeof(CfgData), 1, f);
        fclose(f);
    }
}