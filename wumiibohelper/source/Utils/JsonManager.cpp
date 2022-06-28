#include "JsonManager.hpp"
#include "ArduinoJson.h"

void Utils::JsonManager::ReadGamesIDJson(){
    if(m_gamesidjsondata) return;
    FILE *file = fopen("/3ds/wumiibo/gameids.json", "rb");
    if(!file) /* fallback to romfs jsons */
        file = fopen("romfs:/gameids.json", "rb");
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, 0);
    m_gamesidjsondata = new uint8_t[size];
    fread(m_gamesidjsondata, 1, size, file);
    fclose(file);
}

void Utils::JsonManager::ReadAmiibosJson(){
    if(m_amiibosjsondata) return;
    FILE *file = fopen("/3ds/wumiibo/amiibos.json", "rb");
    if(!file) /* fallback to romfs jsons */
        file = fopen("romfs:/amiibos.json", "rb");
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, 0);
    m_amiibosjsondata = new uint8_t[size];
    fread(m_amiibosjsondata, 1, size, file);
    fclose(file);
}

void Utils::JsonManager::PopulateAmiiboMap(const uint64_t *tids, size_t count){
    DynamicJsonDocument doc1(1 * 1024 * 1024);
    DynamicJsonDocument doc2(1 * 1024 * 1024);
    deserializeJson(doc1, m_gamesidjsondata);
    deserializeJson(doc2, m_amiibosjsondata);

    JsonObject object = doc1.as<JsonObject>();
    for (JsonPair x : object){
        JsonArray y = x.value().as<JsonArray>();
        for(JsonVariant ids : y){
            uint64_t id = strtoll(ids.as<char*>(), NULL, 16);
            for(int i = 0; i < (int)count; i++){
                if(tids[i] == id){
                    JsonArray amiibos = doc2[x.key().c_str()];
                    std::vector <std::pair<std::string, std::string>> vec;
                    for(int j = 0; j < (int)amiibos.size(); j++)
                        vec.push_back(std::make_pair(amiibos[j][0].as<char*>(), amiibos[j][1].as<char*>()));
                    m_amiibomap[id] = vec;
                    break;
                }
            }
        }
    }
}

uint64_t *Utils::JsonManager::GetCompatibleTitles(){
    if(m_compatibletids != nullptr) return m_compatibletids;
    m_compatibletids = new uint64_t[m_amiibomap.size()];
    int i = 0;
    for(auto it = m_amiibomap.begin(); it != m_amiibomap.end(); it++, i++)
        m_compatibletids[i] = it->first;
    return m_compatibletids;
}

std::string Utils::JsonManager::ParseAndGetLatest(const std::vector<uint8_t> &data){
    DynamicJsonDocument doc(1 * 1024 * 1024);
    deserializeJson(doc, (const char*)data.data(), data.size());
    std::string url = doc["assets"][0]["browser_download_url"];
    return url;
}