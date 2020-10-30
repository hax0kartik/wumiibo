#include <3ds.h>

typedef struct 
{
    uint8_t name[0x51];
    uint8_t attributes;
} DirectoryLister_Filename;

class DirectoryLister
{
    public:
        Result PopulateEntries(char *directory);
        Result ListEntries();
        bool HasSelected() {
            return m_selected != -1;
        }
        void ConstructFileLocation();
        char *GetSelectedFileLocation(){
            return m_filename;
        }
        void Reset() {
            m_selected = -1;
            m_readentries = -1;
        }
    private:
        Handle m_fshandle;
        FS_Archive m_archive;
        size_t m_readentries = -1;
        int m_selected = -1;
        DirectoryLister_Filename m_entries[400];
        char m_filename[100];
};