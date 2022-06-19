
#pragma once
#include <string>
#include <3ds.h>

class LumaConfig{
    public:
        void ReadConfig();
        void EnableGamePatching();
        void WriteConfig();
    private:
        std::string m_data;
};