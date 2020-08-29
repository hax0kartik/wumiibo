#include <cinttypes>
#include <cstdio>
#include "amiibo_structs.h"

class TagState
{
    public:
        char *GetTagStateAsStr(int tagstate) {
            char *str = nullptr;
            switch(tagstate)
            {
                case TagStates::Uninitialized:
                    str = "Unitialized";
                    break;
                case TagStates::Scanning:
                    str = "Scanning";
                    break;
                case TagStates::ScanningStopped:
                    str = "Scanning Stopped";
                    break;
                case TagStates::InRange:
                    str = "Tag In Range";
                    break;
                case TagStates::OutOfRange:
                    str = "Tag out of range";
                    break;
                case TagStates::DataReady:
                    str = "DataReady";
                    break;
            }
            return str;
        }

        TagState operator=(int value) {
          //  printf("Changing Tag value from %s to %s\n", GetTagStateAsStr(m_tagstate), GetTagStateAsStr(value));
            m_tagstate = value;
            return *this;
        }

        int Get() {
            // printf("Tag State is %s\n", GetTagStateAsStr(m_tagstate));
            return m_tagstate; 
        }

    private:
        int m_tagstate = 0;
};