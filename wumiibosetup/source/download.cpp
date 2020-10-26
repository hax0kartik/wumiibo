#include "download.hpp"

static size_t write_function(void *ptr, size_t size, size_t nmemb, void *stream) 
{
    std::vector<uint8_t> *data = (std::vector<uint8_t> *)stream;
    uint8_t *u8data = (uint8_t*)ptr;
    for(int i = 0; i < size *nmemb; i++)
        data->push_back(u8data[i]);
    return size * nmemb;
}

Download::Download()
{
    m_handle = curl_easy_init();
    curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(m_handle, CURLOPT_USERAGENT, "Wumiibo");
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_function);
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &m_data);
}

int Download::GetUrl(const std::string &url, std::vector<uint8_t> &data)
{
    data.clear();
    data.shrink_to_fit();
    m_data.clear();
    m_data.shrink_to_fit();
    
    m_url = url;
    curl_easy_setopt(m_handle, CURLOPT_URL, m_url.c_str());
    int res = curl_easy_perform(m_handle);
    if(res == 0)
        data = m_data;
    return res;
}