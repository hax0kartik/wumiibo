#include <vector>
#include <3ds.h>
#include <citro2d.h>
#include <string>
#include "smdh.hpp"
class Titles
{
    public:
        void PopulateTitleArray();
        void PopulateSMDHArray(uint64_t *titles, uint32_t count);
        u64 *GetTitles() { return m_array; };
        u32 GetCount() { return m_count; };
        void ConvertSMDHsToC2D();
        std::vector<C2D_Image> GetC2DSMDHImgs() { return m_images; };
        std::vector<std::string> GetTitlesDescription() { return m_descvector; };
    private:
        uint64_t *m_array;
        uint32_t m_count;
        std::vector<std::vector<uint8_t>> m_smdhvector;
        std::vector<std::string> m_descvector;
        std::vector<C2D_Image> m_images;
        std::vector<C3D_Tex *> m_texs;
        uint64_t m_gamecardid;
};