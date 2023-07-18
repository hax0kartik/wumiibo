#pragma once
#include <optional>
#include <3ds.h>
#include <citro2d.h>
/* forward declaration */
class App;
namespace ui
{
    enum Screen
    {
        Top,
        Bottom
    };

    enum States
    {
        Initial, MainMenu,
        Download, GameSelection, ToggleState,
        AmiiboSelection
    };

    namespace Dimensions
    {
        constexpr float GetHeight()
        {
            return 240;
        }

        constexpr float GetWidth(const Screen screen)
        {
            return screen == Top ? 400 : 320;
        }
    };

    class State
    {
        public:
            virtual std::optional<ui::States> HandleEvent() = 0;
            virtual void OnStateEnter(App *app) = 0;
            virtual void OnStateExit(App *app) = 0;
            virtual void RenderLoop() = 0;
            virtual ~State() = default;
    };

    namespace Elements
    {
        void DrawBars(const Screen s);
        class Icon{
            public:
                void Intialize();
                void DrawIconInMiddle(const Screen s, bool dofadeffect = false);
                static auto &GetInstance(){
                    static Icon m_instance;
                    return m_instance;
                }
                Icon(Icon &other) = delete;
                void operator=(const Icon &) = delete;
            protected:
                Icon() {}
            private:
                C2D_Image m_image;
                float m_alpha = 1.0f;
                uint8_t m_effect = 0;
        };
    }

    Result Intialize();

    class RenderTargets{
        public:
            void CreateRenderTargets(){
                m_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
                m_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
            };
            auto GetRenderTarget(Screen s) const {
                return Screen::Top == s ? m_top : m_bottom;
            };
        private:
            C3D_RenderTarget *m_top;
            C3D_RenderTarget *m_bottom;
    };


    extern RenderTargets g_RenderTarget;
}