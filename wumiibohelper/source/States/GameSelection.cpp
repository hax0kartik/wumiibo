#include "GameSelection.hpp"
#include "../app.hpp"

GameSelection::GameSelection(){
    m_selected = 0;
    m_page = 0;
    m_iconsconverted = 0;
}

GameSelection::~GameSelection(){

}

void GameSelection::OnStateEnter(App *app){
    m_selected = 0;
    m_page = 0;
    m_textbuf = C2D_TextBufNew(2000);

    auto descs = app->GetTitleManager().GetTitleDescription();
    auto titles = app->GetJsonManager().GetCompatibleTitles();

    m_descriptiontexts.resize(descs.size());
    m_amiibostexts.resize(descs.size());

    for(int i = 0; i < (int)descs.size(); i++){
        descs[i] = "Current Title: " + descs[i];
        C2D_TextParse(&m_descriptiontexts[i], m_textbuf, descs[i].c_str());
        C2D_TextOptimize(&m_descriptiontexts[i]);

        auto n = app->GetJsonManager().GetNumberofCompatibleAmiibos(titles[i]);
        std::string s = "Number of compatible figures:" + std::to_string(n);
        C2D_TextParse(&m_amiibostexts[i], m_textbuf, s.c_str());
        C2D_TextOptimize(&m_amiibostexts[i]);
    }

    if(descs.size() <= 0){
        m_descriptiontexts.resize(1);
        m_amiibostexts.resize(1);
        auto str = "No Compatible Titles Found.";
        C2D_TextParse(&m_descriptiontexts[0], m_textbuf, str);
        C2D_TextOptimize(&m_descriptiontexts[0]);
        str = "Press B to go back.";
        C2D_TextParse(&m_amiibostexts[0], m_textbuf, str);
        C2D_TextOptimize(&m_amiibostexts[0]);
        m_broken = true;
    }

    if(m_iconsconverted == 0){
        app->GetTitleManager().ConvertIconsToC2DImage(m_images, m_texs);
        m_iconsconverted = 1;
    }
}

void GameSelection::OnStateExit(App *app){
    m_descriptiontexts.clear();
    m_descriptiontexts.shrink_to_fit();
    m_amiibostexts.clear();
    m_amiibostexts.shrink_to_fit();
    /* Incase of menu, nothing happens */
    app->SetTitle(app->GetJsonManager().GetCompatibleTitles()[m_selected]);
    C2D_TextBufDelete(m_textbuf);
}

std::optional<ui::States> GameSelection::HandleEvent(){
    uint32_t kDown = hidKeysDown();

    if(kDown & KEY_RIGHT)
        m_selected++;
    if(kDown & KEY_LEFT)
        m_selected--;
    if(kDown & KEY_DOWN)
        m_selected += 10;
    if(kDown & KEY_UP)
        m_selected -= 10;

    if(kDown & KEY_B)
        return ui::States::MainMenu;

    if((kDown & KEY_A) && !m_broken)
        return ui::States::AmiiboSelection;

    if(m_selected < 0)
        m_selected = m_descriptiontexts.size() - 1;
    else if(m_selected > (int)m_descriptiontexts.size() - 1)
        m_selected = 0;

    m_page = m_selected / NO_OF_ICONS_PER_PAGE;
    return {};
}

void GameSelection::RenderLoop(){
    auto top = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Top);
    auto bottom = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Bottom);

    /* Top */
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Top);
    ui::Elements::Icon::GetInstance().DrawIconInMiddle(ui::Screen::Top);
    C2D_DrawText(&m_descriptiontexts[m_selected], 0, 10.0f, 190.0f, 1.0f, 0.5f, 0.5f);
    C2D_DrawText(&m_amiibostexts[m_selected], 0, 10.0f, 210.0f, 1.0f, 0.5f, 0.5f);

    /* Bottom */
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Bottom);
    float x = 13.0f, y = 15.0f;
    for(int i = 0; i < NO_OF_ICONS_PER_PAGE && (m_page * NO_OF_ICONS_PER_PAGE) + i < (int)m_images.size(); i++){
        if(x + m_images[(m_page * NO_OF_ICONS_PER_PAGE) + i].subtex->width > 320.0f){
            y = y + m_images[(m_page * NO_OF_ICONS_PER_PAGE) + i].subtex->height + 6.0f;
            x = 13.0f;
        }
        C2D_ImageTint tint;
        if(((m_page * NO_OF_ICONS_PER_PAGE) + i) == m_selected)
            C2D_AlphaImageTint(&tint, 1.0f);
        else
            C2D_AlphaImageTint(&tint, 0.5f);
        C2D_DrawImageAt(m_images[(m_page * NO_OF_ICONS_PER_PAGE) + i], x, y, 1.0f, &tint);
        x += m_images[(m_page * NO_OF_ICONS_PER_PAGE) + i].subtex->width + 6.0f;
    }
}