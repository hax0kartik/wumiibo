#include "MainMenu.hpp"
#include "../Utils/Misc.hpp"


MainMenu::MainMenu(){
    m_selected = 0;

    // Last Edit By: Dylan Sacco
    m_options.push_back("Select Amiibos For Game");
    m_options.push_back("Download Wumiibo");
    // Toggle Status Option (Dynamic)
    bool wumiiboEnabled = Utils::Misc::CheckWumiibo();
    std::string toggleLabel = wumiiboEnabled
        ? "Toggle Wumiibo State (Enabled)"
        : "Toggle Wumiibo State (Disabled)";
    m_options.push_back(toggleLabel);
    
    m_descriptions.push_back("Description: Select Amiibos to use with your games.");
    m_descriptions.push_back("Description: Download selected Amiibos and enables Wumiibo.");
    m_descriptions.push_back("Description: Enables/Disables the Wumiibo system service.");
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

    // Parse and optimize instruction text
    // Added By: Dylan Sacco
    const char* instruction1 = "After downloading Wumiibo (option 2), boot the game";
    const char* instruction2 = "and press (LB + Down + Start) to select an Amiibo";

    C2D_TextParse(&m_instructionTextLine1, m_textbuf, instruction1);
    C2D_TextOptimize(&m_instructionTextLine1);

    C2D_TextParse(&m_instructionTextLine2, m_textbuf, instruction2);
    C2D_TextOptimize(&m_instructionTextLine2);
  
    std::string versionStr = "v" APP_VERSION;
    C2D_TextParse(&m_versiontext, m_textbuf, versionStr.c_str());
    C2D_TextOptimize(&m_versiontext);
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
                return ui::States::GameSelection;
                break;
            case 1:
                return ui::States::Download;
                break;
            case 2:
                return ui::States::ToggleState;
                // When coming back from ToggleState, refresh label
                bool wumiiboEnabled = Utils::Misc::CheckWumiibo();
                m_options[2] = wumiiboEnabled
                    ? "Toggle Wumiibo State (Enabled)"
                    : "Toggle Wumiibo State (Disabled)";
                C2D_TextParse(&m_optiontexts[2], m_textbuf, m_options[2].c_str());
                C2D_TextOptimize(&m_optiontexts[2]);
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

    // Draw instruction text at the very top
    // Added By: Dylan Sacco
    C2D_DrawText(&m_instructionTextLine1, 0, 10.0f, 15.0f, 1.0f, 0.5f, 0.5f);
    C2D_DrawText(&m_instructionTextLine2, 0, 10.0f, 35.0f, 1.0f, 0.5f, 0.5f);

    // Draw description
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
    // Draw version string in top-right corner
    float textWidth, textHeight;
    C2D_TextGetDimensions(&m_versiontext, 0.5f, 0.5f, &textWidth, &textHeight);

    // Adjust X so it hugs the right edge with padding
    float versionY = 10.0f;
    float versionX = ui::Dimensions::GetWidth(ui::Screen::Bottom) - textWidth - 10.0f;
    C2D_DrawText(&m_versiontext, 0, versionX, versionY, 1.0f, 0.5f, 0.5f);
}
