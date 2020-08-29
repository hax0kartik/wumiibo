#pragma once
#include <3ds.h>
#include "amiibo_structs.h"

class AmiiboFile
{
    public:
        Result ReadDecryptedFile(const char *name);
        Result WriteDecryptedFile(const char *name);
        int ParseDecryptedFile();
        void SaveDecryptedFile();
        Amiibo_PlainData *GetPlainData() {
            return &m_plaindata;
        };
        Amiibo_TagInfo *GetTagInfo() {
            return &m_taginfo;
        }
        Amiibo_IdentificationBlock *GetIdentity() {
            return &m_identityblock;
        }
        bool HasParsed() {
            return m_parsed > 0;
        }
        void Reset(){
            m_parsed = 0; // Is Enough
        }
    private:
        uint8_t m_decrypteddata[540];
        Amiibo_PlainData m_plaindata;
        Amiibo_IdentificationBlock m_identityblock;
        Amiibo_TagInfo m_taginfo;
        uint8_t m_parsed = 0;
};