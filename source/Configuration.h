#include <cstring>
#include <3ds.h>
struct key_s
{
    char key[10];
    int val;
};

class Configuration
{
    public:
        Result ReadINI(const char *loc);
        int ParseINI();
        uint32_t GetMenuCombo() { return m_keyval; }
    private:
        char *m_data;
        uint32_t m_keyval = KEY_L | KEY_START | KEY_DDOWN;
        uint8_t m_debug = 0;
};