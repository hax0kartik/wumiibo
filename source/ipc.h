#include <3ds.h>
#include "AmiiboFile.h"
#include "amiibo_structs.h"
#include "nfc.h"

class IPC
{
    public:
        void HandleCommands(NFC *nfc);
    protected:
        void Debug(u32 *cmdbuf, char *str);
    private:
        uint8_t m_hasCalled0xC;
};