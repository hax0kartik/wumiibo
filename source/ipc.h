#include <3ds.h>
#include "AmiiboFile.h"
#include "amiibo_structs.h"
#include "nfc.h"

class IPC
{
    public:
        void HandleCommands(NFC *nfc);
    protected:
        void Debug(u32 *cmdbuf);
    private:
        MyThread m_eventthread;
};