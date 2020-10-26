#include <vector>
#include <string>
#include "app.hpp"
#include "icon_t3x.h"

void DrawBars(bool top, float thickness)
{
    float w, h = 240.0f;
    w = top ? 400.0f : 320.0f;
    C2D_DrawRectSolid (0.0f, 0.0f, 0.0f, w, thickness, 0xff227ee6);
    C2D_DrawRectSolid (0.0f, h - thickness, 0.0f, w, thickness, 0xff227ee6);

}
void DrawTextInCentre(bool top, LightLock *lock, std::string *str)
{
    C2D_Text txt;
    C2D_TextBuf buf = top == true ? ui.top_text_buf : ui.bot_text_buf;
    LightLock_Lock(lock);
    C2D_TextBufClear(buf);
    DrawBars(top, 5.0f);
    C2D_TextParse(&txt, buf, str->c_str());
    //C2D_TextParse(&txt, buf, str->c_str());
    C2D_TextOptimize(&txt);
    float outwidth, outheight;
    C2D_TextGetDimensions (&txt, 1.0f, 1.0f, &outwidth , &outheight);
    C2D_DrawText(&txt, C2D_AlignCenter, top == true ? 200.0f : 160.0f, (240.0f - outheight) / 2, 1.0f, .5f, .5f);
    LightLock_Unlock(lock);
}

float ctr = 0.0f;
void Circle(float center_x, float center_y, float radius)
{
    float point_x = center_x + cos(ctr)*radius;
    float point_y = center_y + sin(ctr)*radius;
    C2D_DrawCircleSolid (point_x, point_y, 1.0f, 1.0f, 0xff000000);
    ctr += 0.1f;
}

void DrawLoadingBarAndText(LightLock *lock, std::string *str)
{
    C2D_Text txt;
    C2D_TextBuf buf = ui.bot_text_buf;
    LightLock_Lock(lock);
    DrawBars(false, 5.0f);
    C2D_TextBufClear(buf);
    C2D_TextParse(&txt, buf, str->c_str());
    C2D_TextOptimize(&txt);
    float outwidth, outheight;
    C2D_TextGetDimensions (&txt, 1.0f, 1.0f, &outwidth , &outheight);
    C2D_DrawText(&txt, C2D_AlignCenter, 160.0f, 120.0f - (outheight/2), 1.0f, 0.5f, 0.5f);
    float circlex, circley;
    float R = 5.0f;
    circlex = 160.0 - R;
    circley = (120.0 - outheight/2) + 45.0 - R;
    Circle(circlex, circley, 5.0f);
    LightLock_Unlock(lock);
}

void DrawMainMenu(LightLock *lock, int *selected, const std::vector<std::string> &options)
{
    LightLock_Lock(lock);
    DrawBars(false, 5.0f);
    C2D_Text txt[2];
    C2D_TextBuf buf = ui.bot_text_buf;
    C2D_TextBufClear(buf);
    float x = 160.0f; float y = 10.0f;
    float outwidth, outheight = 30.0f;
    y = (240.0f - (options.size() * outheight)) / 2;
    for(int i = 0; i < options.size(); i++)
    {
        C2D_TextParse(&txt[i], buf, options[i].c_str());
        C2D_TextOptimize(&txt[i]);
        C2D_TextGetDimensions (&txt[i], 1.0f, 1.0f, &outwidth ,&outheight);
        if(i == *selected)
            C2D_DrawRectSolid (10.0f, y - 5.0f, 0.5f, 300.0f, outheight, 0x50227ee6);
        C2D_DrawText(&txt[i], C2D_AlignCenter, x, y , 1.0f, 0.5f, 0.5f);
        y += outheight;
    }
    LightLock_Unlock(lock);
}

float alpha = 1.0f; u8 effect = 0;
void DrawWumiiboIcon(C2D_Image image, bool dofadeffect)
{
    C2D_ImageTint tint;
    if(effect == 0)
    {
        if(alpha < .5)
            effect = 1;
        alpha = alpha - 0.005f;
    }
    else
    {
        if(alpha > 1.0)
            effect = 0;
        alpha = alpha + 0.005f;
    }
    C2D_AlphaImageTint (&tint, dofadeffect ? alpha : 1.0f);
    C2D_DrawImageAt (image, (400.0f - image.subtex->width) / 2, (240.0f - image.subtex->height) / 2, 1.0f, &tint);
}

void DrawMainMenuTop(LightLock *lock, C2D_Image image, int *selected, const std::vector<std::string> &descs)
{
    LightLock_Lock(lock);
    DrawBars(true, 5.0f);
    DrawWumiiboIcon(image, false);
    C2D_Text txt;
    C2D_TextBuf buf = ui.top_text_buf;
    C2D_TextBufClear(buf);
    C2D_TextParse(&txt, buf, descs[*selected].c_str());
    C2D_TextOptimize(&txt);
    C2D_DrawText(&txt, 0, 10.0f, 210.0f, 1.0f, 0.5f, 0.5f);
    LightLock_Unlock(lock);
}

void DrawTitleInfo(LightLock *lock, C2D_Image image, const std::vector<std::string> &descs, int *selected, int *amiibos)
{
    LightLock_Lock(lock);
    DrawBars(true, 5.0f);
    DrawWumiiboIcon(image, false);
    const char *str = "Current Title: ";
    char str2[35];
    snprintf(str2, 35, "Number of compatible figures: %d", *amiibos);
    C2D_Text txt[3];
    C2D_TextBuf buf = ui.top_text_buf;
    C2D_TextBufClear(buf);
    C2D_TextParse(&txt[0], buf, str);
    C2D_TextParse(&txt[1], buf, descs[*selected].c_str());
    C2D_TextParse(&txt[2], buf, str2);
    float outwidth, outheight;
    C2D_TextGetDimensions (&txt[0], 1.0f, 1.0f, &outwidth ,&outheight);
    C2D_TextOptimize(&txt[0]);
    C2D_TextOptimize(&txt[1]);
    C2D_TextOptimize(&txt[2]);
    C2D_DrawText(&txt[0], 0, 10.0f, 190.0f, 1.0f, 0.5f, 0.5f);
    C2D_DrawText(&txt[1], 0, 10.0f + (outwidth/2), 190.0f, 1.0f, 0.5f, 0.5f);
    C2D_DrawText(&txt[2], 0, 10.0f, 210.0f, 1.0f, 0.5f, 0.5f);
    LightLock_Unlock(lock);
}

void DrawLoadingScreen(LightLock *lock, C2D_Image img)
{
    LightLock_Lock(lock);
    DrawBars(true, 5.0f);
    DrawWumiiboIcon(img, true);
    LightLock_Unlock(lock);
}

int NO_OF_ICONS_PER_PAGE = 10 * 7;
void DrawGameSelectionScreen(LightLock *lock, const std::vector<C2D_Image> &imgs, int *selected, int *page)
{
    LightLock_Lock(lock);
    DrawBars(false, 5.0f);
    float x = 13.0f, y = 15.0f;
    for(int i = 0; i < NO_OF_ICONS_PER_PAGE && (*page * NO_OF_ICONS_PER_PAGE) + i < imgs.size(); i++)
    {
        if(x + imgs[(*page * NO_OF_ICONS_PER_PAGE) + i].subtex->width > 320.0f)
        {
            y = y + imgs[(*page * NO_OF_ICONS_PER_PAGE) + i].subtex->height + 6.0f;
            x = 13.0f;
        }
        C2D_ImageTint tint;
        if(((*page * NO_OF_ICONS_PER_PAGE) + i) == *selected)
            C2D_AlphaImageTint(&tint, 1.0f);
        else
            C2D_AlphaImageTint(&tint, 0.5f);
        C2D_DrawImageAt(imgs[(*page * NO_OF_ICONS_PER_PAGE) + i], x, y, 1.0f, &tint);
        x += imgs[(*page * NO_OF_ICONS_PER_PAGE) + i].subtex->width + 6.0f;
    }
    LightLock_Unlock(lock);
}

void App::DoStuffBeforeMain()
{
    C2D_SpriteSheet sheet = C2D_SpriteSheetLoadFromMem(icon_t3x, icon_t3x_size);
    m_image = C2D_SpriteSheetGetImage(sheet, 0);

    std::string str = "Checking if internet is connected..";
    ui.top_func = std::bind(DrawLoadingScreen, &m_toplock, m_image);
    ui.bot_func = std::bind(DrawLoadingBarAndText, &m_botlock, &str);
    svcSleepThread(2e+9);
    u32 status = 0;
    ACU_GetStatus(&status);
    if(status == 3) 
    {
        LightLock_Lock(&m_botlock);
        str = "Connected.";
        LightLock_Unlock(&m_botlock);
        
        m_utils.ReadGamesIDJson("romfs:/gameids.json");
        m_utils.ReadAmiibosJson("romfs:/amiibos.json");

       // util.DownloadGamesIDJson();
       // util.DownloadAmiibosJson();
    }
    else
    {
        LightLock_Lock(&m_botlock);
        str = "Not Connected. Reading files from romfs..";
        LightLock_Unlock(&m_botlock);

        m_utils.ReadGamesIDJson("romfs:/gameids.json");
        m_utils.ReadAmiibosJson("romfs:/amiibos.json");
    }

    svcSleepThread(1e+9);
    LightLock_Lock(&m_botlock);
    str = "Scanning for compatible titles...";
    LightLock_Unlock(&m_botlock);

    m_titles.PopulateTitleArray();
    m_utils.PopulateAmiiboMap(m_titles.GetTitles(), m_titles.GetCount());
    u32 count = m_utils.GetCompatibleTitlesFoundCount();
    m_tids = m_utils.GetCompatibleTitles();
    m_titles.PopulateSMDHArray(m_tids, count);
    m_titles.ConvertSMDHsToC2D();
    auto icontexvec = m_titles.GetC2DSMDHImgs();

    LightLock_Lock(&m_botlock);
    str = "Titles found: " + std::to_string(count);
    LightLock_Unlock(&m_botlock);

    svcSleepThread(4e+9);
    ui.bot_func = nullptr;
}

void App::MainLoop()
{
    std::vector<std::string> options;
    options.push_back("Download Wumiibo");
    options.push_back("Generate amiibos for game");

    std::vector<std::string> desc;
    desc.push_back("Description: Downloads and installs wumiibo from github.");
    desc.push_back("Description: Generate compatible amiibos for a game.");

    ui.top_func = std::bind(DrawMainMenuTop, &m_toplock, m_image, &m_selected, desc);
    ui.bot_func = std::bind(DrawMainMenu, &m_botlock, &m_selected, options);
    int page = 0;
    int size = 0;
    int amiibos = 0;
    std::string str;
    while(aptMainLoop())
    {
        hidScanInput();

        if(keysDown() & KEY_START)
        { 
            ui.bot_func = ui.top_func = nullptr;
            break;
        }
        if(m_state == 0)
        {
            LightLock_Lock(&m_botlock);

            if(keysDown() & KEY_DOWN)
                m_selected++;
            if(keysDown() & KEY_UP)
                m_selected--;
            
            if(keysDown() & KEY_A)
            {
                ui.bot_func = nullptr;
                m_state = m_selected + 1;
            }
            
            if(m_selected < 0)
                m_selected = options.size() - 1;
            
            else if(m_selected > options.size() - 1)
                m_selected = 0;
            LightLock_Unlock(&m_botlock);
        }

        else if(m_state == 2)
        {
            if(ui.bot_func == nullptr)
            {
                amiibos = m_utils.GetNumberofCompatibleAmiibos(m_tids[m_selected]);
                auto descvec = m_titles.GetTitlesDescription();
                ui.top_func = std::bind(DrawTitleInfo, &m_toplock, m_image, descvec, &m_selected, &amiibos);
                m_selected = 0;
                auto icontexvec = m_titles.GetC2DSMDHImgs();
                size = icontexvec.size();
                ui.bot_func = std::bind(DrawGameSelectionScreen, &m_botlock, icontexvec, &m_selected, &page);
            }

            LightLock_Lock(&m_botlock);
            LightLock_Lock(&m_toplock);

            if(keysDown() & KEY_RIGHT)
                m_selected++;
            if(keysDown() & KEY_LEFT)
                m_selected--;
            if(keysDown() & KEY_DOWN)
                m_selected += 10;
            if(keysDown() & KEY_UP)
                m_selected -= 10;
            
            if(keysDown() & KEY_A)
            {
                ui.bot_func = nullptr;
                m_state = 3;
            }

            if(m_selected < 0)
                m_selected = size - 1;
            
            else if(m_selected > size - 1)
                m_selected = 0;
            
            page = m_selected / NO_OF_ICONS_PER_PAGE;
            amiibos = m_utils.GetNumberofCompatibleAmiibos(m_tids[m_selected]);
            LightLock_Unlock(&m_toplock);
            LightLock_Unlock(&m_botlock);
        }

        else if(m_state == 3)
        {
            if(ui.bot_func == nullptr)
            {
                str = "Generating all amiibos...";
                ui.bot_func = std::bind(DrawLoadingBarAndText, &m_botlock, &str);
                ui.top_func = std::bind(DrawLoadingScreen, &m_toplock, m_image);
                m_utils.GenerateAmiibosForTitle(m_tids[m_selected]);
                svcSleepThread(1e+9);
                str = "Generated! Press B.";
                ui.bot_func = std::bind(DrawTextInCentre, false, &m_botlock, &str);
            }

            if(keysDown() & KEY_B)
            {
                m_state = 2;
                ui.bot_func = nullptr;
            }
        }
    }
}