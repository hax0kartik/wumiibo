#pragma once
#include <cinttypes>

struct Amiibo_IdentificationBlock
{
	uint8_t id[2];
	uint8_t char_variant;
	uint8_t series;
	uint8_t model_no[2];
	uint8_t figure_type;
	uint8_t pad[0x2F];
};

typedef struct{
	uint16_t id_offset_size;/// "uint16_t size/offset of the below ID data. Normally this is 0x7. When this is <=10, this field is the size of the below ID data. When this is >10, this is the offset of the 10-byte ID data, relative to structstart+4+<offsetfield-10>. It's unknown in what cases this 10-byte ID data is used."
	uint8_t unk_x2;//"Unknown uint8_t, normally 0x0."
	uint8_t unk_x3;//"Unknown uint8_t, normally 0x2."
	uint8_t id[0x28];//"ID data. When the above size field is 0x7, this is the 7-byte Amiibo tag UID, followed by all-zeros."
}Amiibo_TagInfo;

struct Date
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	Date()
	{
		year = 0; month = 0; day = 0;
	}
	Date(uint16_t _year, uint8_t _month, uint8_t _day)
	{
		year = _year;
		month = _month;
		day = _day;
	}
    Date(uint16_t raw)
    {
        year = (raw >> 9) + 2000;
    	month = (raw << 23 >> 28) & 0xF;
        day = raw & 0x1F;
    };

    uint16_t getraw()
    {
        return (((year << 9) + 0x6000) | 0x20 * month) | day;
    }
};

/// AmiiboSettings structure, see also here: https://3dbrew.org/wiki/Amiibo:GetAmiiboSettings
typedef struct {
	uint8_t mii[0x60];/// "Owner Mii."
	uint16_t nickname[11];/// "UTF-16BE Amiibo nickname."
	uint8_t flags;/// "This is plaintext_amiibosettingsdata[0] & 0xF." See also the Amiibo_amiiboFlag enums.
	uint8_t countrycodeid;/// "This is plaintext_amiibosettingsdata[1]." "Country Code ID, from the system which setup this amiibo."
	Date setupdate;
	uint8_t unk_x7c[0x2c];//Normally all-zero?
} Amiibo_AmiiboSettings;

/// AmiiboConfig structure, see also here: https://3dbrew.org/wiki/Amiibo:GetAmiiboConfig
typedef struct {
	Date lastwritedate;
	uint16_t write_counter;
	uint8_t characterID[3];/// the first element is the collection ID, the second the character in this collection, the third the variant
	uint8_t series;/// ID of the series
	uint8_t amiiboID[2];/// ID shared by all exact same amiibo. Some amiibo are only distinguished by this one like regular SMB Series Mario and the gold one
	uint8_t type;/// Type of amiibo 0 = figure, 1 = card, 2 = plush
	uint8_t pagex4_byte3;
	uint16_t appdata_size;/// "Amiibo module writes hard-coded uint8_t value 0xD8 here. This is the size of the Amiibo AppData, apps can use this with the AppData R/W commands. ..."
	uint8_t zeros[0x30];/// "Unused / reserved: this is cleared by Amiibo module but never written after that."
} Amiibo_AmiiboConfig;

typedef struct
{
	uint64_t titleid; // TitleID
	uint32_t appid; // AppID
	uint16_t counter; // Counter
	uint8_t unk; // this is plaintextamiibosettingsdata >> 4
	uint8_t unk2; // this is always set to 2
	uint8_t tid_related; // this is tid related. set to 0.
	uint8_t unk3[0x2F]; // More unknown fields
}Amiibo_AppDataConfig;


typedef struct 
{
	uint8_t pagex4_byte3;
	uint8_t flag;
	Date lastwritedate;
	uint16_t writecounter;
	Amiibo_AmiiboSettings settings;
	Amiibo_AppDataConfig appDataConfig;
	uint8_t AppData[0xd8];
} Amiibo_PlainData;

enum TagStates {
    Uninitialized = 0, /// AmiiboInit() was not used yet.
	ScanningStopped = 1, /// Not currently scanning for Amiibo tags. Set by AmiiboStopScanning() and AmiiboInit(), when successful.
    Scanning = 2, /// Currently scanning for Amiibo tags. Set by AmiiboStartScanning() when successful.
	InRange = 3, /// Amiibo tag is in range. The state automatically changes to this when the state was previously value 2, without using any Amiibo service commands.
	OutOfRange = 4, /// Amiibo tag is now out of range, where the Amiibo tag was previously in range. This occurs automatically without using any Amiibo service commands. Once this state is entered, it won't automatically change to anything else when the tag is moved in range again. Hence, if you want to keep doing tag scanning after this, you must stop+start scanning.
	DataReady = 5 /// Amiibo tag data was successfully loaded. This is set by AmiiboLoadAmiiboData() when successful.
};