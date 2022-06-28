#include "Toggle.hpp"
#include "../app.hpp"
#include "../Utils/Misc.hpp"

Toggle::Toggle(){
}

Toggle::~Toggle(){
}

void Toggle::OnStateEnter(App *app){
    m_textbuf = C2D_TextBufNew(100);
    auto havewumiibo = Utils::Misc::CheckWumiibo();
    Utils::Misc::SetWumiiboState(!havewumiibo);
    if(havewumiibo){
        SetString("Disabled. Press B to Reboot.");
    } else {
        SetString("Enabled. Press B to Reboot.");
    }
}

void Toggle::OnStateExit(App *app){
    (void)app;
    C2D_TextBufDelete(m_textbuf);
}

std::optional<ui::States> Toggle::HandleEvent(){
    if(keysDown() & KEY_B)
        Utils::Misc::Reboot();
    return {};
}

void Toggle::RenderLoop(){
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
    auto y = (ui::Dimensions::GetHeight() - 15.0f) / 2;
    C2D_DrawText(&m_text, C2D_AlignCenter, 160.0f, y, 1.0f, 0.5f, 0.5f);
}

void Toggle::SetString(const std::string &str){
    m_message = str;
    C2D_TextParse(&m_text, m_textbuf, m_message.c_str());
    C2D_TextOptimize(&m_text);
}