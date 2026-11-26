#include "global.h"
struct Slot1745 {
    u32 words[8];
};
struct Entry1745 {
    u8 pad0[0x1C];
    struct Slot1745 slot;
    u8 pad3C[0x44 - 0x3C];
};
struct Header1745 {
    u8 pad0[0x1A];
    u16 count;
};
struct Table1745 {
    u32 pad0;
    struct Slot1745 slots[6];
    u8 padC4[0xE0 - 0xC4];
    struct Entry1745 entries[1];
};
// confidence=low: copies each entry's 32-byte slot into the compact slot array.
void sub_08138630(struct Header1745 *header, struct Table1745 *table)
{
    s32 i;
    for (i = 0; i < header->count; i++) {
        struct Entry1745 *e = &table->entries[i];
        table->slots[i] = e->slot;
    }
}
