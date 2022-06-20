#include <vector>
#include <map>
#include <utility>
#include <string>
#include <3ds.h>
#include "download.hpp"
#include "ArduinoJson.h"

class Utils
{
    public:
        void ReadGamesIDJson(std::string loc);
        void ReadAmiibosJson(std::string loc);
        void DownloadGamesIDJson();
        void DownloadAmiibosJson();
        void DownloadAndExtractLatestReleaseZip();
        void PopulateAmiiboMap(const uint64_t *tids, size_t count);
        void GenerateAmiibos();
        void GenerateAmiibosForTitle(u64 tid);
        int GetCompatibleTitlesFoundCount() {return m_amiibomap.size();};
        uint64_t *GetCompatibleTitles();
        int GetNumberofCompatibleAmiibos(uint64_t tid) { return m_amiibomap[tid].size(); };
        void Reboot();
        void RebootToSelf();
        bool IsReboot();
        bool CheckWumiibo();
        bool SetWumiiboState(bool newstate);
    private:
        uint8_t *m_gamesidjsondata;
        uint8_t *m_amiibosjsondata;
        Download m_download;
        uint64_t *m_compatibletids = nullptr;
        std::map<uint64_t, std::vector<std::pair<std::string, std::string>>> m_amiibomap;
};