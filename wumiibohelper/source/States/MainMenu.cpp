#include "MainMenu.hpp"

MainMenu::MainMenu(){
    m_selected = 0;

    m_options.push_back("Download Wumiibo");
    m_options.push_back("Generate amiibos for game");
    m_options.push_back("Toggle Wumiibo State");

    m_descriptions.push_back("Description: Downloads and installs latest wumiibo.");
    m_descriptions.push_back("Description: Generate compatible amiibos for a game.");
    m_descriptions.push_back("Description: Enable/Disable Wumiibo");
}

MainMenu::~MainMenu() {}

void MainMenu::OnStateEnter(App *app){
    (void)app;
    m_selected = 0;
    m_textbuf = C2D_TextBufNew(1000);
    m_optiontexts.resize(m_options.size());
    m_descriptiontexts.resize(m_descriptions.size());
    for(int i = 0; i < (int)m_options.size(); i++){
        C2D_TextParse(&m_optiontexts[i], m_textbuf, m_options[i].c_str());
        C2D_TextOptimize(&m_optiontexts[i]);
    }
    for(int i = 0; i < (int)m_descriptions.size(); i++){
        C2D_TextParse(&m_descriptiontexts[i], m_textbuf, m_descriptions[i].c_str());
        C2D_TextOptimize(&m_descriptiontexts[i]);
    }
}

void MainMenu::OnStateExit(App *app){
    (void)app;
    m_optiontexts.clear();
    m_optiontexts.shrink_to_fit();
    m_descriptiontexts.clear();
    m_descriptiontexts.shrink_to_fit();
    C2D_TextBufDelete(m_textbuf);
}

std::optional<ui::States> MainMenu::HandleEvent(){
    auto kDown = hidKeysDown();
    if(kDown & KEY_DOWN)
        m_selected++;

    if(kDown & KEY_UP)
        m_selected--;

    if(kDown & KEY_A){
        switch(m_selected){
            case 0:
                return ui::States::Download;
                break;
            case 1:
                return ui::States::GameSelection;
                break;
            case 2:
                return ui::States::ToggleState;
                break;
        }
    }

    if(m_selected < 0)
        m_selected = m_options.size() - 1;
    else if(m_selected > m_options.size() - 1)
        m_selected = 0;
    return {};
}


void MainMenu::RenderLoop(){
    auto top = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Top);
    auto bottom = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Bottom);

    /* Top */
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Top);
    ui::Elements::Icon::GetInstance().DrawIconInMiddle(ui::Screen::Top);
    C2D_DrawText(&m_descriptiontexts[m_selected], 0, 10.0f, 210.0f, 1.0f, 0.5f, 0.5f);

    /* Bottom */
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Bottom);
    auto y = (ui::Dimensions::GetHeight() - (m_optiontexts.size() * 30.0f)) / 2;
    for(int i = 0; i < (int)m_optiontexts.size(); i++)
    {
        if(i == m_selected)
            C2D_DrawRectSolid (10.0f, y - 5.0f, 0.5f, ui::Dimensions::GetWidth(ui::Screen::Bottom) - 20.0f, 30.0f, 0x50227ee6);
        C2D_DrawText(&m_optiontexts[i], C2D_AlignCenter, 160.0f, y, 1.0f, 0.5f, 0.5f);
        y += 30.0f;
    }
}