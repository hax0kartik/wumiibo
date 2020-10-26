#include <curl/curl.h>
#include <string>
#include <vector>

class Download{
    public:
        Download();
        int GetUrl(const std::string &url, std::vector<uint8_t> &data);
    private:
        CURL *m_handle;
        std::string m_url;
        std::vector<uint8_t> m_data;
};