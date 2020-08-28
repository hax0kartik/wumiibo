#include <3ds.h>

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
    private:
        Handle m_fshandle;
        FS_Archive m_archive;
        size_t m_readentries = -1;
        int m_selected = -1;
        FS_DirectoryEntry m_entries[10];
        char m_filename[100];
};