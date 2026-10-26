#include "global.h"
#include "render.h"

/* Sprite resource bank (header at 0x03004300): entries of 12 bytes from +0x18. */
struct AnimResourceEntry {
    u16 id;
    u8 field2;
    u8 pad3;
    u8 field4, field5, field6, field7;
    u32 offset;
};
struct AnimResourceHeader {
    u32 pad0;
    s32 count;
    u8 pad8[4];
    u32 offsetC;
    u32 offset10;
    u8 pad14[4];
    struct AnimResourceEntry entries[1];
};
struct AnimData {
    u8 field0, field1;
    u16 field2;
    u16 field4;
    u16 palette;
    u8 field8, field9, fieldA, fieldB;
    u8 *paletteData;
    u8 *data10;
    u8 *data14;
};
extern struct AnimResourceHeader *gUnk_03004300;
/* Fills anim from the resource entry with this id; 0 if there is none. */
s32 AnimData_Load(struct AnimData *anim, u32 id)
{
    struct AnimResourceHeader *header = gUnk_03004300;
    u8 *base10, *baseC;
    struct AnimResourceEntry *entry;
    u16 *info;
    s32 found, i;
    if (!header)
        return 0;
    baseC = (u8 *)header + header->offsetC;
    base10 = (u8 *)header + header->offset10;
    found = 0;
    entry = header->entries;
    for (i = 0; i < gUnk_03004300->count; entry++, i++) {
        if (entry->id == id) {
            found = 1;
            break;
        }
    }
    if (!found)
        return 0;
    info = (u16 *)(entry->offset + base10);
    anim->field0 = entry->field2;
    anim->field1 = 0;
    anim->field2 = 0;
    anim->field8 = entry->field4;
    anim->field9 = entry->field5;
    anim->fieldA = entry->field6;
    anim->fieldB = entry->field7;
    anim->field4 = 0;
    anim->palette = info[1];
    anim->paletteData = gPaletteBankData + anim->palette * 32;
    anim->data10 = ((u32 *)info)[1] + baseC;
    anim->data14 = entry->offset + base10;
    return 1;
}
