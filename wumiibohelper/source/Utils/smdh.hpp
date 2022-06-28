#pragma once
#include <3ds.h>

typedef struct {
    u16 shortdescription[0x40];
    u16 longdescription[0x80];
    u16 publisher[0x40];
} SMDH_title;

typedef struct {
    char magic[0x04];
    u16 version;
    u16 reserved1;
    SMDH_title titles[0x10];
    u8 ratings[0x10];
    u32 region;
    u32 matchmakerid;
    u64 matchmakerbitid;
    u32 flags;
    u16 eulaversion;
    u16 reserved;
    u32 optimalbannerframe;
    u32 streetpassId;
    u64 reserved2;
    u8 smallicon[0x480];
    u8 largeicon[0x1200];
} SMDH;