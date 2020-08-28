#include <3ds.h>
#include "AmiiboFile.h"
#include "amiibo_structs.h"
#include "TagState.h"
#include "nfc.h"

class IPC
{
    public:
        void HandleCommands(NFC *nfc);
    protected:
        void Debug(u32 *cmdbuf);
    private:
        TagState m_tagstate;
        Handle m_taginrange = -1;
        Handle m_tagoutofrange = -1;
        AmiiboFile m_file;
        char filename[100];
};