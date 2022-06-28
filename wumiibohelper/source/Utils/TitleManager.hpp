#pragma once
#include <vector>
#include <string>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

namespace Utils{
    class TitleManager{
        public:
            void operator=(const TitleManager &) = delete;
            void PopulateTitleList();
            uint64_t *GetTitleList(){ return m_titles; };
            auto GetSize(){ return m_count; };
            void PopulateIcons(uint64_t *titles, uint32_t count);
            auto& GetTitleDescription(){ return m_descvector; };
            void ConvertIconsToC2DImage(std::vector<C2D_Image> &images, std::vector<C3D_Tex*> &texs);

        private:
            uint64_t *m_titles;
            uint32_t m_count;
            uint64_t m_gamecardid;
            std::vector<std::vector<uint8_t>> m_smdhvector;
            std::vector<std::string> m_descvector;
    };
}