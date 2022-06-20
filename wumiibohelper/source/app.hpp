#include <3ds.h>
#include "ui.hpp"
#include "utils.hpp"
#include "print.hpp"
#include "titles.hpp"
class App
{
    public:
        void DoStuffBeforeMain();
        void MainLoop();
    private:
        Result m_res;
        Utils m_utils;
        int m_state = 0;
        LightLock m_toplock;
        LightLock m_botlock;
        std::string m_name = "Wumiibo Amiibo Generator\n";
        Titles m_titles;
        int m_selected = 0;
        bool m_connected = false;
        C2D_Image m_image;
        uint64_t *m_tids;
        bool m_havewumiibo = false;
};