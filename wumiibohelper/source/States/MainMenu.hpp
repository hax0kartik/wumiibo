#pragma once
#include <vector>
#include <string>
#include <optional>
#include "../ui.hpp"

class MainMenu : public ui::State{
    public:
        MainMenu();
        ~MainMenu() override;
        std::optional<ui::States> HandleEvent() override;
        void OnStateEnter(App *app) override;
        void OnStateExit(App *app) override;
        void RenderLoop() override;

    private:
        int m_selected = 0;
        std::vector<std::string> m_options;
        std::vector<std::string> m_descriptions;

        /* UI */
        C2D_TextBuf m_textbuf;
        std::vector<C2D_Text> m_optiontexts;
        std::vector<C2D_Text> m_descriptiontexts;

        // Instruction text for mod menu hint
        C2D_Text m_instructionTextLine1;
        C2D_Text m_instructionTextLine2;
        C2D_Text m_versiontext;
};
