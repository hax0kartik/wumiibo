#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string>

namespace Utils{
    class JsonManager{
        public:
            void operator=(const JsonManager &) = delete;
            void ReadGamesIDJson();
            void ReadAmiibosJson();
            void PopulateAmiiboMap(const uint64_t *tids, size_t count);
            uint64_t *GetCompatibleTitles();
            auto GetCompatibleTitlesCount() { return m_amiibomap.size(); };
            auto GetNumberofCompatibleAmiibos(uint64_t tid) { return m_amiibomap[tid].size(); };
            auto& GetAmiibosForTitles(uint64_t tid) { return m_amiibomap[tid]; };
            std::string ParseAndGetLatest(const std::vector<uint8_t> &data);
        private:
            uint8_t *m_gamesidjsondata = nullptr, *m_amiibosjsondata = nullptr;
            std::map<uint64_t, std::vector<std::pair<std::string, std::string>>> m_amiibomap;
            uint64_t *m_compatibletids = nullptr;
    };
}