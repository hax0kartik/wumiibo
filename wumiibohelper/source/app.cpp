#include <vector>
#include <string>
#include <utility>
#include <malloc.h>
#include "app.hpp"

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
    C2D_Text txt[3];
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

void DrawAmiiboSelectionScreen(LightLock *lock, const std::vector<std::pair<std::string, std::string>>& options, int *selected, int *page)
{
    LightLock_Lock(lock);
    DrawBars(false, 5.0f);
    const int size = (int)options.size();
    C2D_Text txt[15]; // 15 is the max element on screen
    C2D_TextBuf buf = ui.bot_text_buf;
    C2D_TextBufClear(buf);
    float x = 0.0f; float y = 7.5f;
    float outwidth, outheight = 30.0f;
    for(int i = 0; i < 15 && (*page * 15) + i < size; i++)
    {
        int entry = (*page * 15) + i;
        C2D_TextParse(&txt[i], buf, std::get<0>(options[entry]).c_str());
        C2D_TextOptimize(&txt[i]);
        C2D_TextGetDimensions (&txt[i], 1.0f, 1.0f, &outwidth, &outheight);
        if(entry == *selected)
            C2D_DrawRectSolid (0.0f, y, 0.5f, 320.0f, (outheight/2), 0x50227ee6);
        C2D_DrawText(&txt[i], C2D_AlignLeft, x, y, 1.0f, 0.5f, 0.5f);
        y += (outheight/2);
    }
    LightLock_Unlock(lock);
}

void DrawAmiiboIconTop(LightLock *lock, const std::vector<C2D_Image>& icons, int *selected)
{
    LightLock_Lock(lock);
    DrawBars(true, 5.0f);
    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, 1.0f);
    int entry = *selected % 15;
    if (icons.size() > entry)
    {
        const auto image = icons[entry];
        C2D_DrawImageAt(image, (400.0f - image.subtex->width) / 2, (240.0f - image.subtex->height) / 2, 1.0f, &tint);
    }
    LightLock_Unlock(lock);
}

void App::DoStuffBeforeMain()
{
    C2D_SpriteSheet sheet = C2D_SpriteSheetLoad("/3ds/wumiibo/images/icon.t3x");
    m_image = C2D_SpriteSheetGetImage(sheet, 0);

    std::string str = "Checking if internet is connected..";
    ui.top_func = std::bind(DrawLoadingScreen, &m_toplock, m_image);
    ui.bot_func = std::bind(DrawLoadingBarAndText, &m_botlock, &str);
   // svcSleepThread(2e+9);
    u32 status = 0;
   // ACU_GetStatus(&status);
    if(status == 3) 
    {
        u32 *socbuf = (u32*)memalign(0x1000, 0x100000);
        socInit(socbuf, 0x100000);
        m_connected = true;
        LightLock_Lock(&m_botlock);
        str = "Connected.\nRetrieving data from github...";
        LightLock_Unlock(&m_botlock);
        
        m_utils.DownloadGamesIDJson();
        m_utils.DownloadAmiibosJson();
    }

    m_utils.ReadGamesIDJson();
    m_utils.ReadAmiibosJson();

   // svcSleepThread(1e+9);
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
    //svcSleepThread(4e+9);
    ui.bot_func = nullptr;
}

void App::MainLoop()
{
    const std::vector<std::string> options
    {
        "Download Wumiibo",
        "Generate amiibos for game",
        "Toggle Wumiibo State"
    };

    const std::vector<std::string> desc
    {
        "Description: Downloads and installs latest wumiibo.",
        "Description: Generate compatible amiibos for a game.",
        "Description: Enable/Disable Wumiibo."
    };
    int page = 0, prevpage = 0, selected = 0;
    int size = 0;
    int amiibos = 0;
    std::vector<std::pair<std::string, std::string>> amiibo;
    std::vector<C2D_Image> amiiboimages;
    std::string str;

    if(m_utils.IsReboot())
        m_state = 5;

    m_havewumiibo = m_utils.CheckWumiibo();
   
    while(aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDownRepeat();
        hidSetRepeatParameters(10, 5);
        if(kDown & KEY_START)
        { 
            ui.bot_func = ui.top_func = nullptr;
            break;
        }
        if(m_state == 0)
        {
            if(ui.bot_func == nullptr)
            {
                ui.top_func = std::bind(DrawMainMenuTop, &m_toplock, m_image, &m_selected, desc);
                ui.bot_func = std::bind(DrawMainMenu, &m_botlock, &m_selected, options);
            }

            LightLock_Lock(&m_botlock);

            if(kDown & KEY_DOWN)
                m_selected++;
            if(kDown & KEY_UP)
                m_selected--;
            
            if(kDown & KEY_A)
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
        else if(m_state == 1)
        {
            if(ui.bot_func == nullptr)
            {
                if(!m_connected)
                {
                    str = "Not Connected to Internet. Press B.";
                    ui.bot_func = std::bind(DrawTextInCentre, false, &m_botlock, &str);
                    continue;
                }
                str = "Downloading latest release...";
                ui.bot_func = std::bind(DrawLoadingBarAndText, &m_botlock, &str);
                ui.top_func = std::bind(DrawLoadingScreen, &m_toplock, m_image);
                m_utils.DownloadAndExtractLatestReleaseZip();
                str = "Download Complete. Rebooting...";
                ui.bot_func = std::bind(DrawTextInCentre, false, &m_botlock, &str);
                svcSleepThread(2e+9);
                m_utils.RebootToSelf();
            }

            if(kDown & KEY_B)
            {
                ui.bot_func = nullptr;
                m_state = 0;
            }
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

            if(kDown & KEY_RIGHT)
                m_selected++;
            if(kDown & KEY_LEFT)
                m_selected--;
            if(kDown & KEY_DOWN)
                m_selected += 10;
            if(kDown & KEY_UP)
                m_selected -= 10;
            
            if(kDown & KEY_A)
            {
                ui.bot_func = nullptr;
                m_state = 4;
            }

            if(kDown & KEY_B)
            {
                ui.bot_func = nullptr;
                m_state = 0;
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
                if(m_havewumiibo)
                    str = "Disabling wumiibo..";
                else
                    str = "Enabling wumiibo..";
                ui.bot_func = std::bind(DrawLoadingBarAndText, &m_botlock, &str);
                ui.top_func = std::bind(DrawLoadingScreen, &m_toplock, m_image);
                svcSleepThread(1e+9);
                if(m_utils.SetWumiiboState(!m_havewumiibo))
                    str = "Done. Press B to exit.";
                else
                    str = "Error Occured. Do you have wumiibo installed?";
                ui.bot_func = std::bind(DrawTextInCentre, false, &m_botlock, &str);
            }

            if(kDown & KEY_B)
            {
                m_utils.Reboot();
                ui.bot_func = nullptr;
            }
        }

        else if(m_state == 4)
        {
            if(ui.bot_func == nullptr)
            {
                page = 0;
                prevpage = 0;
                selected = m_selected;
                m_selected = 0;
                amiibo = m_utils.GetAmiibosForTitle(m_tids[selected]);
                m_utils.LoadAmiiboImagesForTitle(m_tids[selected], page, 15);
                amiiboimages = m_utils.GetAmiiboImages();
                ui.top_func = std::bind(DrawAmiiboIconTop, &m_toplock, amiiboimages, &m_selected);
                ui.bot_func = std::bind(DrawAmiiboSelectionScreen, &m_botlock, amiibo, &m_selected, &page);
            }

            if(prevpage != page)
            {
                LightLock_Lock(&m_toplock);
                m_utils.LoadAmiiboImagesForTitle(m_tids[selected], page, 15);
                amiiboimages = m_utils.GetAmiiboImages();
                ui.top_func = std::bind(DrawAmiiboIconTop, &m_toplock, amiiboimages, &m_selected);
                LightLock_Unlock(&m_toplock);
            }

            LightLock_Lock(&m_botlock);
            LightLock_Lock(&m_toplock);

            if(kDown & KEY_DOWN)
                m_selected++;
            if(kDown & KEY_UP)
                m_selected--;

            if(m_selected < 0)
                m_selected = amiibo.size() - 1;
            
            else if(m_selected > amiibo.size() - 1)
                m_selected = 0;

            prevpage = page;
            page = m_selected / 15;
            LightLock_Unlock(&m_toplock);
            LightLock_Unlock(&m_botlock);

            if(kDown & KEY_B)
            {
                m_state = 2;
                ui.bot_func = nullptr;
            }
        }

        else if(m_state == 5)
        {
            if(ui.bot_func == nullptr)
            {
                str = "Verifying installation...";
                ui.top_func = std::bind(DrawLoadingScreen, &m_toplock, m_image);
                ui.bot_func = std::bind(DrawTextInCentre, false, &m_botlock, &str);
                if(m_havewumiibo)
                    str = "Install Complete! Press B to exit.";
                else
                    str = "Install Failed!";
                ui.bot_func = std::bind(DrawTextInCentre, false, &m_botlock, &str);
            }

            if(kDown & KEY_B)
            {
                m_utils.Reboot();
                ui.bot_func = nullptr;
            }
        }
    }
    if(m_connected) socExit();
}