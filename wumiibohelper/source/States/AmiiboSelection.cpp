#include <cstdio>
#include "AmiiboSelection.hpp"
#include "../app.hpp"
#include "../Amiibos.hpp"
#include "../Utils/Misc.hpp"

static void LoadImagesThread(AmiiboSelection& amiiboselection, App *app){
    auto tid = app->GetTitle();
    auto& amiibos = app->GetJsonManager().GetAmiibosForTitles(tid);
    while(!amiiboselection.IsDone()){
        LightEvent_Wait(amiiboselection.GetImageThreadEvent());
        auto& amiiboimages = amiiboselection.GetAmiiboImages();
        auto& amiibosprites = amiiboselection.GetAmiiboSprites();
        for(auto& sprite : amiibosprites){
            if(sprite)
                C2D_SpriteSheetFree(sprite);
        }
        amiibosprites = {};
        amiiboimages.clear();
        amiiboimages.shrink_to_fit();
        auto& page = amiiboselection.GetPage();
        auto size = std::min(14, (int)(amiibos.size() - page * 14));
        auto entry = page * 14;
        for(int i = entry; i < entry + size; i++){
            auto id = std::get<1>(amiibos[i]).substr(2);
            auto loc = "romfs:/images/" + id + ".t3x";
            auto sheet = C2D_SpriteSheetLoad(loc.c_str());
            auto image = C2D_SpriteSheetGetImage(sheet, 0);
            amiiboimages.push_back(image);
            amiibosprites.push_back(sheet);
        }
    }
}

static void CreateBinThread(AmiiboSelection& amiiboselection, App *app){
    auto tid = app->GetTitle();
    auto& amiibos = app->GetJsonManager().GetAmiibosForTitles(tid);
    bool done = false;
    while(!amiiboselection.IsDone()){
        auto& queue = amiiboselection.GetQueue();
        if(queue.Size() == 0){
            if(done){
                amiiboselection.SetString("Generated!");
                svcSleepThread(0.5e+9);
                amiiboselection.SetString(" ");
                done = false;
            }
        } else {
            auto selected = queue.Dequeue();
            auto name = std::get<0>(amiibos[selected]);
            auto id = std::get<1>(amiibos[selected]).substr(2);
            auto loc = "/wumiibo";
            std::string s = "Generating bin for " + name + " ...";
            amiiboselection.SetString(s);
            char hex[20];
            sprintf(hex, "%016llX", tid);
            Utils::Misc::CreateBin(name, id, loc, hex);
            done = true;
        }
        svcSleepThread(0.005e+9);
    }
}

AmiiboSelection::AmiiboSelection(){
    LightEvent_Init(&m_event, RESET_ONESHOT);
    LightLock_Init(&m_lock);
    m_page = 0;
    m_prevpage = 0;
    m_selected = 0;
}

AmiiboSelection::~AmiiboSelection(){
}

void AmiiboSelection::OnStateEnter(App *app){
    m_page = 0;
    m_prevpage = 0;
    m_selected = 0;
    m_done = false;
    hidSetRepeatParameters(10, 20);
    
    auto tid = app->GetTitle();
    m_amiibos = app->GetJsonManager().GetAmiibosForTitles(tid);
    m_textbuf = C2D_TextBufNew(8000);
    m_optiontexts.resize(m_amiibos.size());
    for(int i = 0; i < (int)m_amiibos.size(); i++){
        std::string amiiboname = std::get<0>(m_amiibos[i]);
        C2D_TextParse(&m_optiontexts[i], m_textbuf, amiiboname.c_str());
        C2D_TextOptimize(&m_optiontexts[i]);
    }
    /* This is probably better to do every frame but idc */
    m_pagestexts.resize((m_amiibos.size() / 14) + 1);
    int i = 0;
    do{
        std::string page = "Page: " + std::to_string(i + 1) + "/" + std::to_string(m_pagestexts.size());
        C2D_TextParse(&m_pagestexts[i], m_textbuf, page.c_str());
        C2D_TextOptimize(&m_pagestexts[i]);
        i++;
    }while(i < m_pagestexts.size());

    /* Select an amiibo + Amiibo Name + Type + Series */
    m_extrastexts.resize(4);
    std::string s = "Select an amiibo:";
    C2D_TextParse(&m_extrastexts[0], m_textbuf, s.c_str());
    C2D_TextOptimize(&m_extrastexts[0]);

    /* -- */
    m_gameseriestext.resize(g_gamesseriesmap.size());
    for(int i = 0; i < g_gamesseriesmap.size(); i++){
        s = "Game Series: " + g_gamesseriesmap[i];
        C2D_TextParse(&m_gameseriestext[i], m_textbuf, s.c_str());
        C2D_TextOptimize(&m_gameseriestext[i]);
    }
    m_amiibotypetext.resize(g_type.size());
    for(int i = 0; i < g_type.size(); i++){
        s = "Type: " + g_type[i];
        C2D_TextParse(&m_amiibotypetext[i], m_textbuf, s.c_str());
        C2D_TextOptimize(&m_amiibotypetext[i]);
    }

    SetString(" ");
    worker.CreateThread(LoadImagesThread, *this, app, 4 * 1024 * 1024);
    worker1.CreateThread(CreateBinThread, *this, app);
    LightEvent_Signal(&m_event);
}

void AmiiboSelection::OnStateExit(App *app){
    (void)app;
    m_done = true;
    /* Signal the event once because thread is stuck waiting on it */
    LightEvent_Signal(&m_event); 
    /* Wait for both threads to exit */
    while(!worker1.IsDone() && !worker.IsDone()){
        svcSleepThread(0.5e+9);
    }
    m_optiontexts.clear();
    m_optiontexts.shrink_to_fit();
    m_extrastexts.clear();
    m_extrastexts.shrink_to_fit();
    m_pagestexts.clear();
    m_pagestexts.shrink_to_fit();
    m_amiiboimages.clear();
    m_amiiboimages.shrink_to_fit();
    C2D_TextBufDelete(m_textbuf);
}

std::optional<ui::States> AmiiboSelection::HandleEvent(){
    uint32_t kDown = hidKeysDown() | hidKeysDownRepeat();

    if(kDown & KEY_A){
        m_queue.Enqueue(m_selected);
    }
    if(kDown & KEY_B)
        return ui::States::GameSelection;
    if(kDown & KEY_DOWN)
        m_selected++;
    if(kDown & KEY_UP)
        m_selected--;

    if(m_selected < 0)
        m_selected = m_optiontexts.size() - 1;

    else if(m_selected > m_optiontexts.size() - 1)
        m_selected = 0;

    m_prevpage = m_page;
    m_page = m_selected / 14;
    if(m_page != m_prevpage)
        LightEvent_Signal(&m_event);
    return {};
}

void AmiiboSelection::RenderLoop(){
    auto top = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Top);
    auto bottom = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Bottom);

    /* Top */
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Top);
    auto screenwidth = ui::Dimensions::GetWidth(ui::Screen::Top);
    auto screenheight = ui::Dimensions::GetHeight();
    C2D_DrawText(&m_pagestexts[m_page], 0, 10.0f, 210.0f, 1.0f, 0.5f, 0.5f);
    LightLock_Lock(&m_lock);
    C2D_DrawText(&m_extrastexts[2], 0, 160.0f, 210.0f, 1.0f, 0.5f, 0.5f);
    LightLock_Unlock(&m_lock);
    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, 1.0f);
    C2D_DrawText(&m_optiontexts[m_selected], 0, 160.0f, (screenheight - (15.0f * 3)) / 2, 1.0f, 0.5f, 0.5f);
    auto type = std::get<1>(m_amiibos[m_selected]).substr(8, 2);
    int n = strtol(type.c_str(), nullptr, 16);
    C2D_DrawText(&m_amiibotypetext[n], 0, 160.0f, (screenheight - (15.0f * 2)) / 2 + 5.0f, 1.0f, 0.5f, 0.5f);
    auto gameseriesid = std::get<1>(m_amiibos[m_selected]).substr(14, 2);
    n = strtol(gameseriesid.c_str(), nullptr, 16);
    C2D_DrawText(&m_gameseriestext[n], 0, 160.0f, (screenheight - 15.0f) / 2 + 10.0f, 1.0f, 0.5f, 0.5f);

    if((int)m_amiiboimages.size() > (m_selected % 14)){
        auto image = m_amiiboimages[m_selected % 14];
        C2D_DrawImageAt(image, 20.0f, (screenheight - image.subtex->height) / 2, 1.0f, &tint);
    } else {
        ui::Elements::Icon::GetInstance().DrawIconInMiddle(ui::Screen::Top, true);
    }
    /* Bottom */
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Bottom);
    C2D_DrawText(&m_extrastexts[0], C2D_AlignCenter, 160.0f, 5.0f, 1.0f, 0.5f, 0.5f);
    auto y = 20.0f, x = 5.0f;
    for(int i = 0; i < 14 && (m_page * 14) + i < (int)m_optiontexts.size(); i++){
        int entry = (m_page * 14) + i;
        if(entry == m_selected)
            C2D_DrawRectSolid (0.0f, y, 0.5f, 320.0f, 15.0f, 0x50227ee6);
        C2D_DrawText(&m_optiontexts[entry], C2D_AlignLeft, x, y, 1.0f, 0.5f, 0.5f);
        y += 15.0f;
    }
}

void AmiiboSelection::SetString(const std::string &str){
    LightLock_Lock(&m_lock);
    m_message = str;
    C2D_TextParse(&m_extrastexts[2], m_textbuf, m_message.c_str());
    C2D_TextOptimize(&m_extrastexts[2]);
    LightLock_Unlock(&m_lock);
}