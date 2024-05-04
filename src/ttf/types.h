#pragma once

#include "../utils.h"

typedef char Tag[5];

Result *Bitstream_readTag(Bitstream *self, Tag buf);

#define ENUM_PARSE(_bs, _typ, _typRead, _typEnum, _var)                                                                \
  {                                                                                                                    \
    _typ id;                                                                                                           \
    TRY(Bitstream_read##_typRead(_bs, &id));                                                                           \
    _var = (_typEnum)id;                                                                                               \
  }

typedef struct {
  u16 advanceWidth;
  i16 lsb; // Glyph left side bearing
} LongHorMetric;

typedef enum {
  PlatformID_Unicode = 0,
  PlatformID_Macintosh,
  PlatformID_ISO,
  PlatformID_Windows,
  PlatformID_Custom,
} PlatformID;

const char *PlatformID_string(PlatformID platform);

typedef enum {
  EncodingIDUnicode_1_0 = 0,
  EncodingIDUnicode_1_1,
  EncodingIDUnicode_ISO_IEC_10646,
  EncodingIDUnicode_2_0_BMP,
  EncodingIDUnicode_2_0_Full,
  EncodingIDUnicode_Variation,
  EncodingIDUnicode_Full,
} EncodingIDUnicode;

typedef enum {
  EncodingIDWindows_Symbol = 0,
  EncodingIDWindows_UnicodeBMP,
  EncodingIDWindows_ShiftJIS,
  EncodingIDWindows_PRC,
  EncodingIDWindows_Big5,
  EncodingIDWindows_Wansung,
  EncodingIDWindows_Johab,
  EncodingIDWindows_Reserved7,
  EncodingIDWindows_Reserved8,
  EncodingIDWindows_Reserved9,
  EncodingIDWindows_UnicodeFull,
} EncodingIDWindows;

typedef enum {
  EncodingIDMacintosh_Roman = 0,
} EncodingIDMacintosh;

typedef union {
  EncodingIDUnicode unicode;
  EncodingIDMacintosh mac;
  EncodingIDWindows windows;
} EncodingID;

Result *EncodingID_parse(PlatformID *platform, EncodingID *encoding, Bitstream *bs);

typedef enum {
  TableTag_GDEF = 0,
  TableTag_GPOS,
  TableTag_GSUB,
  TableTag_OS2,
  TableTag_Cmap,
  TableTag_Cvt,
  TableTag_Fpgm,
  TableTag_Gasp,
  TableTag_Head,
  TableTag_Loca,
  TableTag_Maxp,
  TableTag_Name,
  TableTag_Hmtx,
  TableTag_Hhea,
  TableTag_Glyf,

  TableTag_Unknown
} TableTag;

Result *TableTag_parse(TableTag *tableTag, Bitstream *bs);

typedef enum {
  LocFormat_Short = 0,
  LocFormat_Long = 1,
} LocFormat;

typedef struct {
  u32 checksum;
  u32 offset;
  u32 length;
} TableRecord;

Result *TableRecord_parse(TableRecord *record, Bitstream *bs);

typedef struct {
  PlatformID platformID;
  EncodingID encodingID;
  u32 subtableOffset;
} EncodingRecord;

Result *EncodingRecord_parse(EncodingRecord *self, Bitstream *bs);

typedef struct {
  u16 rangeMaxPPEM;
  u16 rangeGaspBehavior;
} GaspRange;

Result *GaspRange_parse(GaspRange *self, Bitstream *bs);

typedef enum {
  NameID_Copyright = 0,
  NameID_FontFamilyName,
  NameID_FontSubFamilyName,
  NameID_UniqueFontIdentifier,
  NameID_FullFontName,
  NameID_VersionString,
  NameID_PostScriptName,
  NameID_Trademark,
  NameID_ManufacturerName,
  NameID_Designer,
  NameID_Description,
  NameID_URLVendor,
  NameID_URLDesigner,
  NameID_LicenseDescription,
  NameID_LicenseInfoURL,
  NameID_Reserved,
  NameID_TypographicFamilyName,
  NameID_TypographicSubfamilyName,
  NameID_CompatibleFull,
  NameID_SampleText,
  NameID_PostScriptCIDFindFontName,
  NameID_WWSFamilyName,
  NameID_WWSSubfamilyName,
  NameID_LightBackgroundPalette,
  NameID_DarkBackgroundPalette,
  NameID_VariationsPostScriptNamePrefix,
  NameID_Unknown,
} NameID;

typedef struct {
  PlatformID platformID;
  EncodingID encodingID;
  u16 languageID;
  NameID nameID;
  u16 length;
  u16 stringOffset;
} NameRecord;

Result *NameRecord_parse(NameRecord *self, Bitstream *bs);

typedef struct {
  u16 length;
  u16 langTagOffset;
} LangTagRecord;

Result *LangTagRecord_parse(LangTagRecord *self, Bitstream *bs);
