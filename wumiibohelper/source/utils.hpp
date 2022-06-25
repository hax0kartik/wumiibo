#include <vector>
#include <map>
#include <utility>
#include <string>
#include <3ds.h>
#include <citro2d.h>
#include "download.hpp"
#include "ArduinoJson.h"

class Utils
{
    public:
        void ReadGamesIDJson();
        void ReadAmiibosJson();
        void DownloadGamesIDJson();
        void DownloadAmiibosJson();
        void DownloadAndExtractLatestReleaseZip();
        void PopulateAmiiboMap(const uint64_t *tids, size_t count);
        void GenerateAmiibos();
        void GenerateAmiibosForTitle(u64 tid);
        std::vector<std::pair<std::string, std::string>>& GetAmiibosForTitle(u64 tid);
        int GetCompatibleTitlesFoundCount() {return m_amiibomap.size();};
        uint64_t *GetCompatibleTitles();
        int GetNumberofCompatibleAmiibos(uint64_t tid) { return m_amiibomap[tid].size(); };
        void Reboot();
        void RebootToSelf();
        bool IsReboot();
        bool CheckWumiibo();
        bool SetWumiiboState(bool newstate);
        void LoadAmiiboImagesForTitle(u64 tid, int page, size_t pagesize);
        std::vector<C2D_Image> &GetAmiiboImages() { return m_amiiboimages; };
    private:
        uint8_t *m_gamesidjsondata;
        uint8_t *m_amiibosjsondata;
        Download m_download;
        uint64_t *m_compatibletids = nullptr;
        std::map<uint64_t, std::vector<std::pair<std::string, std::string>>> m_amiibomap;
        std::vector<C2D_Image> m_amiiboimages;
        std::vector<C2D_SpriteSheet> m_sheets;
};