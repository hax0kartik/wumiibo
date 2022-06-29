#include "Initial.hpp"
#include "../app.hpp"

Initial::Initial(){
    LightLock_Init(&m_lock);
}

Initial::~Initial(){
}

void Initial::OnStateEnter(App *app){
    m_textbuf = C2D_TextBufNew(1000);
    std::string str = "Initial State";
    SetString(str);
    m_broken = false;
    worker.CreateThread([](Initial& initial, App *app) -> void{
        auto& jsonmanager = app->GetJsonManager();
        auto& titlemanager = app->GetTitleManager();
        auto& downloadmanager = app->GetDownloadManager();

        std::string str = "Checking if Internet is connected.";
        if(app->IsConnected()){
            str = "Connected.\nRetrieving data from github..";
            initial.SetString(str);
            downloadmanager.DownloadAmiibosJson();
            downloadmanager.DownloadGamesIDJson();
        }
        str = "Loading Data..";
        initial.SetString(str);
        jsonmanager.ReadAmiibosJson();
        jsonmanager.ReadGamesIDJson();
        titlemanager.PopulateTitleList();
        auto titles = titlemanager.GetTitleList();
        auto titlelistsize = titlemanager.GetSize();
        jsonmanager.PopulateAmiiboMap(titles, titlelistsize);
        auto filteredtitles = jsonmanager.GetCompatibleTitles();
        auto filteredtitlescount = jsonmanager.GetCompatibleTitlesCount();
        str = "Titles found:" + std::to_string(filteredtitlescount);
        initial.SetString(str);
        titlemanager.PopulateIcons(filteredtitles, filteredtitlescount);
    }, *this, app);
}

void Initial::OnStateExit(App *app){
    (void)app;
    C2D_TextBufDelete(m_textbuf);
}

std::optional<ui::States> Initial::HandleEvent(){
    if(worker.IsDone() && !m_broken)
        return ui::States::MainMenu;
    return {};
}

void Initial::RenderLoop(){
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

void Initial::SetString(const std::string &str){
    LightLock_Lock(&m_lock);
    m_message = str;
    C2D_TextParse(&m_text, m_textbuf, m_message.c_str());
    C2D_TextOptimize(&m_text);
    LightLock_Unlock(&m_lock);
}