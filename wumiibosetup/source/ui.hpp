#pragma once
#include <functional>
#include <3ds.h>
#include <citro2d.h>

typedef struct 
{
    std::function<void()> top_func;
    std::function<void()> bot_func;
    C2D_TextBuf bot_text_buf;
    C2D_TextBuf top_text_buf;
    C2D_Font font;
    bool debug;
    bool done;
    C3D_RenderTarget *top;
    C3D_RenderTarget *bottom;
    Handle event;
}uiFuncs_s;

void uiThread(void *arg);

extern uiFuncs_s ui;