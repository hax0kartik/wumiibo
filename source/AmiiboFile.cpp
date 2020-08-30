#include <cstring>
#include "bswap.h"
#include "AmiiboFile.h"

Result AmiiboFile::ReadDecryptedFile(const char *name)
{
   Handle fshandle;
   Result ret = FSUSER_OpenFileDirectly(&fshandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, NULL), fsMakePath(PATH_ASCII, name), FS_OPEN_READ, 0);
   if(ret) return ret;
   ret = FSFILE_Read(fshandle, NULL, 0, m_decrypteddata, 540);
   if(ret) return ret;
   ret = FSFILE_Close(fshandle);
   if(ret) return ret;
   return 0;
}

Result AmiiboFile::WriteDecryptedFile(const char *name)
{
   Handle fshandle;
   Result ret = FSUSER_OpenFileDirectly(&fshandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, NULL), fsMakePath(PATH_ASCII, name), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);
   if(ret) return ret;
   ret = FSFILE_Write(fshandle, NULL, 0, m_decrypteddata, 540, 0);
   if(ret) return ret;
   ret = FSFILE_Close(fshandle);
   if(ret) return ret;
   return 0;
}


int AmiiboFile::ParseDecryptedFile()
{
   if(m_decrypteddata[0x0C] == 0xF1 && m_decrypteddata[0x0D] == 0x10)
      return -1;

   if(m_decrypteddata[0x02] != 0xF && m_decrypteddata[0x3] != 0xE0)
      return -2;
   
   memcpy((u8*)&m_identityblock, (u8*)&m_decrypteddata[0x1DC], 8);
   m_plaindata.pagex4_byte3 = m_decrypteddata[0x2B];
   m_plaindata.flag = m_decrypteddata[0x2C];
   m_plaindata.lastwritedate = Date(bswap_16(*(uint16_t*)&m_decrypteddata[0x32]));
   m_plaindata.writecounter = bswap_16((m_decrypteddata[0xB4] << 8) | m_decrypteddata[0xB5]);
   if(m_plaindata.flag << 27 >> 31)
   {
      memcpy(m_plaindata.settings.mii, &m_decrypteddata[0x4C], 0x60);
      memcpy(m_plaindata.settings.nickname, &m_decrypteddata[0x38], 2*10);
      m_plaindata.settings.flags = m_decrypteddata[0x2C] & 0xF;
      m_plaindata.settings.countrycodeid = m_decrypteddata[0x2D];
      m_plaindata.settings.setupdate = Date(bswap_16(*(uint16_t*)&m_decrypteddata[0x30]));
      if(m_plaindata.flag << 26 >> 31)
      {
         memcpy((uint8_t*)&m_plaindata.appDataConfig.appid, (uint8_t*)&m_decrypteddata[0xB6], 4);
         memcpy((uint8_t*)&m_plaindata.appDataConfig.titleid, (uint8_t*)&m_decrypteddata[0xAC], 8);
         m_plaindata.appDataConfig.titleid = bswap_64(m_plaindata.appDataConfig.titleid);
         m_plaindata.appDataConfig.counter = bswap_16(*((uint16_t*)&m_decrypteddata[0xB4]));
         m_plaindata.appDataConfig.unk = m_plaindata.flag >> 4;
         memcpy((uint8_t*)&m_plaindata.AppData[0], (uint8_t*)&m_decrypteddata[0xDC], 0xD8);
      }
   }
   m_parsed = 1;
   return 0;
}

void AmiiboFile::SaveDecryptedFile()
{
   m_decrypteddata[0x2B] = m_plaindata.pagex4_byte3;
   m_decrypteddata[0x2C] = m_plaindata.flag;
   // memcpy(&m_decrypteddata[0x32], &m_plaindata.lastwritedate.getraw(), 2); //TODO fix this
   m_plaindata.writecounter = bswap_16(m_plaindata.writecounter += 1);
   memcpy(&m_decrypteddata[0xB4], &m_plaindata.writecounter, 2);
   if(m_plaindata.flag << 27 >> 31)
   {
      memcpy(&m_decrypteddata[0x4C], m_plaindata.settings.mii, 0x60);
      memcpy(&m_decrypteddata[0x38], m_plaindata.settings.nickname, 2*10);
      m_decrypteddata[0x2D] = m_plaindata.settings.countrycodeid;
      uint16_t date = bswap_16(m_plaindata.settings.setupdate.getraw());
      memcpy(&m_decrypteddata[0x30], &date, 2);
      if(m_plaindata.flag << 26 >> 31)
      {
         memcpy((uint8_t*)&m_decrypteddata[0xB6], (uint8_t*)&m_plaindata.appDataConfig.appid, 4);
         memcpy((uint8_t*)&m_decrypteddata[0xAC], (uint8_t*)&m_plaindata.appDataConfig.titleid, 8);
         //m_plaindata.appDataConfig.counter = bswap_16(*((uint16_t*)&m_decrypteddata[0xB4]));
         //m_plaindata.appDataConfig.unk = m_plaindata.flag >> 4;
         memcpy((uint8_t*)&m_decrypteddata[0xDC], (uint8_t*)&m_plaindata.AppData[0], 0xD8);
      }
   }
}