#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include "utils.hpp"

void Utils::ReadGamesIDJson(std::string loc)
{
    FILE *file = fopen(loc.c_str(), "rb");
    int size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, 0);
	m_gamesidjsondata = new uint8_t[size];
	fread(m_gamesidjsondata, 1, size, file);
    printf("[*] size of m_gamesidjson is %d\n", size);
	fclose(file);
}

void Utils::ReadAmiibosJson(std::string loc)
{
    FILE *file = fopen(loc.c_str(), "rb");
    int size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, 0);
	m_amiibosjsondata = new uint8_t[size];
	fread(m_amiibosjsondata, 1, size, file);
    printf("[*] size of m_amiibosjson is %d\n", size);
	fclose(file);
}

void Utils::PopulateAmiiboMap(const uint64_t *tids, size_t count)
{
    DynamicJsonDocument doc1(1 * 1024 * 1024);
	DynamicJsonDocument doc2(1 * 1024 * 1024);
	deserializeJson(doc1, m_gamesidjsondata);
	deserializeJson(doc2, m_amiibosjsondata);

	JsonObject object = doc1.as<JsonObject>();
	for (JsonPair x : object)
	{
		JsonArray y = x.value().as<JsonArray>();
		for(JsonVariant ids : y)
		{
			uint64_t id = strtoll(ids.as<char*>(), NULL, 16);
			for(int i = 0; i < count; i++)
			{
				if(tids[i] == id)
				{
					JsonArray amiibos = doc2[x.key().c_str()];
					std::vector <std::pair<std::string, std::string>> vec;
					for(int j = 0; j < amiibos.size(); j++)
						vec.push_back(std::make_pair(amiibos[j][0].as<char*>(), amiibos[j][1].as<char*>()));
					m_amiibomap[id] = vec;
					break;		
				}
			}
		}
	}
}

void CreateBin(std::string name, std::string amiiboid, std::string loc)
{
    //printf("Name %s\n", name.c_str());
    //printf("AmiiboID %s\n", amiiboid.c_str());
    //printf("Location %s\n", loc.c_str());

    std::string filename = loc + "/" + name + ".bin";
    u64 amid = strtoll(amiiboid.c_str(), NULL, 16);
    FILE *file = fopen(filename.c_str(), "r");
    if(file)
    {
        fclose(file);
        for(int i = 1; ;i++)
        {
            filename = loc + "/" + name + "_" + std::to_string(i) + ".bin";
            file = fopen(filename.c_str(), "r");
            if(!file) break;
            fclose(file);
        }
    }
    file = fopen(filename.c_str(), "wb");
    if(!file)
        return;
    char *data = new char[540];
    memset(data, 0, 540);
    data[0x2] = 0x0F;
    data[0x3] = 0xE0;
    u8 *amiddata = (u8*)&amid;
    int offset = 0x1DC;
    for(int i = 7; i >= 0; i--)
    {
        data[offset++] = amiddata[i];
    }
    fwrite(data, 540, 1, file);
    fclose(file);
    delete[] data;
    return;
}

uint64_t *Utils::GetCompatibleTitles()
{
    if(m_compatibletids != nullptr) return m_compatibletids;
    m_compatibletids = new uint64_t[m_amiibomap.size()];
    int i = 0;
    for(auto it = m_amiibomap.begin(); it != m_amiibomap.end(); it++, i++)
        m_compatibletids[i] = it->first;
    return m_compatibletids;
}

void Utils::GenerateAmiibos()
{
    std::string loc = "/wumiibo/";
    std::string file_loc;
    char hex[20];
    for (auto it = m_amiibomap.begin(); it != m_amiibomap.end(); it++)
	{
		printf("ID %llX\n", it->first);
        mkdir(loc.c_str(), 0777);
        sprintf(hex, "%016llX", it->first);
        file_loc = loc + hex;
        mkdir(file_loc.c_str(), 0777);
		for(auto pair : it->second)
        {
            std::string id = std::get<1>(pair).substr(2);
			CreateBin(std::get<0>(pair), id, file_loc);
        }
	}
    return;
}

void Utils::GenerateAmiibosForTitle(u64 tid)
{
    std::string loc = "/wumiibo/";
    std::string file_loc;
    char hex[20];
    auto vec = m_amiibomap[tid];
    mkdir(loc.c_str(), 0777);
    sprintf(hex, "%016llX", tid);
    file_loc = loc + hex;
    mkdir(file_loc.c_str(), 0777);
    for(auto pair : vec)
    {
        std::string id = std::get<1>(pair).substr(2);
        CreateBin(std::get<0>(pair), id, file_loc);
    }
    return;
}