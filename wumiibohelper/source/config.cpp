#include "config.hpp"
#include <cstdio>

void LumaConfig::ReadConfig()
{
    FILE *f = fopen("/luma/config.ini", "r");
    if(f)
    {
        fseek(f, 0L, SEEK_END);
        size_t size = ftell(f);
        m_data.resize(size);
        fseek(f, 0L, SEEK_SET);
        fread(&m_data[0], size, 1, f);
        fclose(f);
    }
}

void LumaConfig::EnableGamePatching()
{
    const std::string s = "enable_game_patching = 1";
    auto found = m_data.find("enable_game_patching");
    if(found != std::string::npos){
        m_data.replace(found, s.length(), s);
    }
}

void LumaConfig::WriteConfig()
{
    FILE *f = fopen("/luma/config.ini", "wb+");
    if(f)
    {
        fwrite(&m_data[0], m_data.length(), 1, f);
        fclose(f);
    }
}