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
            if(m_tagstate == TagStates::InRange)
                m_inrangecounter++;
            else
                m_inrangecounter = 0;

            if(m_inrangecounter > 10) // Game is definately stuck in a loop waiting for tagstate to change
            {
                m_tagstate = TagStates::OutOfRange;
                m_inrangecounter = 0;
            }
            
            return m_tagstate; 
        }

    private:
        uint8_t m_inrangecounter = 0;
        int m_tagstate = 0;
};