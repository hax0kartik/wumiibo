#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "logger.h"
#include "ifile.h"

#define LOG_FILE "/wumiibo.txt"

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING
static int logger_started = 0;
static IFile f;
static char buffer[1500];
#endif // ENABLE_LOGGING

void logInit() {
	#if ENABLE_LOGGING
    if (logger_started) {
        return;
    }

    Result res = IFile_Open(&f, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, LOG_FILE), FS_OPEN_CREATE | FS_OPEN_WRITE | FS_OPEN_READ);
    if (R_FAILED(res)) {
		logger_started = -1;
		return;
    }

	u64 total;
	IFile_Write(&f, &total, "----\n", 5, FS_WRITE_FLUSH);

    logger_started = 1;
    #endif // ENABLE_LOGGING
}


void logPrintf(const char *format, ...) {
	#if ENABLE_LOGGING
    va_list args;
    va_start(args, format);

	vsnprintf(buffer, 1500, format, args);

	logStr(buffer);

    va_end(args);
    #endif // ENABLE_LOGGING
}

void logStr(const char *str) {
	#if ENABLE_LOGGING
    if (logger_started <= 0) {
        return;
    }

	u64 total;
	IFile_Write(&f, &total, str, strlen(str), FS_WRITE_FLUSH);
	#endif // ENABLE_LOGGING
}

void logBuf(char *prefix, u8* data, size_t len) {
	#if ENABLE_LOGGING
	char bufstr[len*3 + 3];
	memset(bufstr, 0, sizeof(bufstr));
	for(int pos=0; pos<len; pos++) {
		snprintf(&bufstr[pos*3], 4, "%02x ", data[pos]);
		if (pos > 0 && pos % 12 == 0) {
			bufstr[pos*3+2] = '\n';
		}
	}
	logStr(prefix);
	logStr(" hex: ");
	logStr(bufstr);
	logStr("\n");
	#endif // ENABLE_LOGGING
}

void logExit() {
	#if ENABLE_LOGGING
    if (logger_started <= 0)
        return;

    IFile_Close(&f);
    logger_started = 0;
    #endif // ENABLE_LOGGING
}

void logCrash(const char *str) {
	#if ENABLE_LOGGING
    logStr(str);
    logExit();
    #endif // ENABLE_LOGGING
    svcBreak(USERBREAK_ASSERT);
}

