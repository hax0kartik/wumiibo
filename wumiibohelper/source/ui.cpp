#include "ui.hpp"

ui::RenderTargets ui::g_RenderTarget;
Result ui::Intialize(){
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    ui::g_RenderTarget.CreateRenderTargets();
    ui::Elements::Icon::GetInstance().Intialize();
    return 0;
}

void ui::Elements::Icon::Intialize(){
    C2D_SpriteSheet sheet = C2D_SpriteSheetLoad("/3ds/wumiibo/images/icon.t3x");
    if(!sheet){
        sheet = C2D_SpriteSheetLoad("romfs:/icon.t3x");
    }
    m_image = C2D_SpriteSheetGetImage(sheet, 0);
}

void ui::Elements::Icon::DrawIconInMiddle(const ui::Screen s, bool dofadeffect){
    C2D_ImageTint tint;
    if(m_effect == 0){
        if(m_alpha < .5)
            m_effect = 1;
        m_alpha = m_alpha - 0.005f;
    } else {
        if(m_alpha > 1.0)
            m_effect = 0;
        m_alpha = m_alpha + 0.005f;
    }
    auto screenwidth = ui::Dimensions::GetWidth(s);
    constexpr auto screenheight = ui::Dimensions::GetHeight();
    C2D_AlphaImageTint (&tint, dofadeffect ? m_alpha : 1.0f);
    C2D_DrawImageAt (m_image, (screenwidth - m_image.subtex->width) / 2, (screenheight - m_image.subtex->height) / 2, 1.0f, &tint);
}

void ui::Elements::DrawBars(const ui::Screen screen){
    const auto thickness = 5.0f;
    auto screenwidth = ui::Dimensions::GetWidth(screen);
    constexpr auto screenheight = ui::Dimensions::GetHeight();
    C2D_DrawRectSolid(0.0f, 0.0f, 0.0f, screenwidth, thickness, 0xff227ee6);
    C2D_DrawRectSolid(0.0f, screenheight - thickness, 0.0f, screenwidth, thickness, 0xff227ee6);
}