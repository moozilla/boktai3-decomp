#ifndef GUARD_PASSWORD_H
#define GUARD_PASSWORD_H

#include "engine.h"

/* Working state for decoding a typed password / name string (0x90 bytes, heap allocated). */
struct PasswordEntry {
    u8 codes[24];   /* 6 bit symbol indices (output of Password_EncodeSymbols) */
    u8 text[48];    /* typed characters with tags removed (24 pairs) */
    u8 decoded[44]; /* decoded payload words */
    u8 name[5];     /* decoded name characters, 0xF0 = substituted glyph */
    u8 pad79[3];
    u8 *table;  /* glyph pair table (0x40 pairs) */
    int script; /* string table id used to resolve 0xF0 */
    int version;
    int checksum;
    int key;
};

int Text_CopyWithoutTags(u8 *dst, u8 *src, int limit);
s32 GlyphTable_FindPair(u8 *p, u32 a, u32 b);
s32 Password_EncodeSymbols(struct PasswordEntry *e);
int sub_080115DC(struct PasswordEntry *e);
int NameEntry_DecodeAndValidate(struct PasswordEntry *e);
void Password_ApplyDecoded(struct PasswordEntry *e, u8 *output);
s32 sub_08011794(u8 *src, s32 tableRef, s32 script, u8 *output);

int sub_08044CF4(void *, int *, int, int);
int sub_08221594(void *, int, int);
void sub_080453DC(void *, void *, int);

#endif
