#pragma once
#include <vector>
#include <string>
#include <optional>
#include <cstdio>
#include "../ui.hpp"
#include "../workerthread.hpp"
#include "../Utils/DownloadManager.hpp"
class Initial : public ui::State{
    public:
        Initial();
        ~Initial() override;
        std::optional<ui::States> HandleEvent() override;
        void OnStateEnter(App *app) override;
        void OnStateExit(App *app) override;
        void RenderLoop() override;

        void SetString(const std::string& str);
        void SetBroken(bool broken) { m_broken = broken; };

    private:
        std::string m_message;
        LightLock m_lock;
        bool m_broken = false;
        /* UI */
        C2D_TextBuf m_textbuf;
        C2D_Text m_text;
        WorkerThread<Initial& ,App*> worker; 
};