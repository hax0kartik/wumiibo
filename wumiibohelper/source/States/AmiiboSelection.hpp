#pragma once
#include <vector>
#include <string>
#include <optional>
#include <utility>
#include "../ui.hpp"
#include "../workerthread.hpp"
#include "../threadsafequeue.hpp"

class AmiiboSelection : public ui::State{
    public:
        AmiiboSelection();
        ~AmiiboSelection() override;
        std::optional<ui::States> HandleEvent() override;
        void OnStateEnter(App *app) override;
        void OnStateExit(App *app) override;
        void RenderLoop() override;

        bool IsDone() { return m_done; };
        LightEvent *GetImageThreadEvent() { return &m_event; };
        auto& GetPage() { return m_page; };
        auto& GetQueue() { return m_queue; };
        auto& GetAmiiboImages() {return m_amiiboimages; };
        void SetString(const std::string &str);
    private:
        int m_page = 0, m_prevpage = 0;
        int m_selected = 0, m_oldselected = 0;
        int m_done = 0;
        std::vector<std::pair<std::string, std::string>> m_amiibos;
        LightEvent m_event;
        ThreadSafeQueue<int> m_queue;
        std::string m_message;
        /* UI */
        LightLock m_lock;
        C2D_TextBuf m_textbuf;
        std::vector<C2D_Text> m_optiontexts;
        std::vector<C2D_Text> m_pagestexts;
        std::vector<C2D_Text> m_extrastexts;
        std::vector<C2D_Image> m_amiiboimages;
        WorkerThread<AmiiboSelection&, App*> worker;  /* ImageLoaderThread */
        WorkerThread<AmiiboSelection&, App*> worker1; /* BinGeneratorThread */
};