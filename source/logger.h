#pragma once

void setLogEnable(bool val);
bool isLogEnabled();
void logInit();
void logPrintf(const char *format, ...);
void logStr(const char *str);
void logBuf(char *prefix, u8* data, size_t len);
void logExit();
void logCrash(const char *str);