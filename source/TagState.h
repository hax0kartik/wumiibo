#include <cinttypes>
#include <cstdio>
class TagState
{
    public:
        TagState operator=(int value)
        {
           // printf("Changing Tag value from %d to %d\n", m_tagstate, value);
            m_tagstate = value;
            return *this;
        }

        int Get(){ return m_tagstate; }

    private:
        int m_tagstate = 0;
};