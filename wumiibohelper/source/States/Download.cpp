#include "Download.hpp"
#include "../app.hpp"
#include "../Utils/Misc.hpp"

Download::Download(){
    LightLock_Init(&m_lock);
}

Download::~Download(){

}

void Download::OnStateEnter(App *app){
    m_textbuf = C2D_TextBufNew(500);
    std::string s;
    if(app->IsReboot()){
        m_fullreboot = true;
        m_rebootrequired = false;
        auto havewumiibo = Utils::Misc::CheckWumiibo();
        if(havewumiibo)
            s = "Download Complete. Press B to Exit the app.\n" \
            "Note: Your Real figurines will not work\n with wumiibo enabled.\n" \
            "You can disable wumiibo to\n continue using your real figurines.";
        else
            s = "Verification failed.\nPlease manually install the app.\nPress B to exit.";
        SetString(s);
        return;
    }
    if(!app->IsConnected()){
        m_rebootrequired = false;
        m_fullreboot = false;
        s = "Not connected to Internet. Press B.";
        SetString(s);
        return;
    }
    s = "Downloading Latest Release...";
    SetString(s);
    worker.CreateThread([](Download &download, App *app) -> void {
        auto& downloadmanager = app->GetDownloadManager();
        auto& jsonmanager = app->GetJsonManager();
        std::vector<uint8_t> tmp;
        downloadmanager.GetUrl("https://api.github.com/repos/hax0kartik/wumiibo/releases/latest", tmp);
        auto url = jsonmanager.ParseAndGetLatest(tmp);
        downloadmanager.DownloadTo(url, "/luma/sysmodules", "0004013000004002.cxi");
        Utils::Misc::EnableGamePatching();
        std::string s = "Download Complete. Rebooting...";
        download.SetString(s);
        svcSleepThread(2e+9); // 2 secs
    }, *this, app, 1024 * 1024 * 2); /* 2 MB should be enough */
    m_rebootrequired = true;
}

void Download::OnStateExit(App *app){
    (void)app;
    while(!worker.IsDone()){
        svcSleepThread(0.05e+9);
    }
    C2D_TextBufDelete(m_textbuf);
    if(m_fullreboot){
        Utils::Misc::Reboot();
        while(1);;
    }
    if(m_rebootrequired){
        app->Finalize();
        Utils::Misc::RebootToSelf();
        while(1);;
    }
}

std::optional<ui::States> Download::HandleEvent(){
    /* We mark it as going to menu when infact we'll reboot */
    if(worker.IsDone() && m_rebootrequired)
        return ui::States::MainMenu;

    if(keysDown() & KEY_B)
        return ui::States::MainMenu;
    return {};
}

void Download::RenderLoop(){
    auto top = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Top);
    auto bottom = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Bottom);

    /* Top */
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Top);
    ui::Elements::Icon::GetInstance().DrawIconInMiddle(ui::Screen::Top, true);

    /* Bottom */
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Bottom);
    auto height = 0.0f, width = 0.0f;
    C2D_TextGetDimensions(&m_text, 0.5f, 0.5f, &width, &height);
    auto y = (ui::Dimensions::GetHeight() - height) / 2;
    LightLock_Lock(&m_lock);
    C2D_DrawText(&m_text, C2D_AlignCenter, 160.0f, y, 1.0f, 0.5f, 0.5f);
    LightLock_Unlock(&m_lock);
}

void Download::SetString(const std::string &str){
    LightLock_Lock(&m_lock);
    m_message = str;
    C2D_TextParse(&m_text, m_textbuf, m_message.c_str());
    C2D_TextOptimize(&m_text);
    LightLock_Unlock(&m_lock);
}