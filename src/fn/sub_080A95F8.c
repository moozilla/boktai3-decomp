#include "global.h"


struct Entry080A9 {
    u32 field0;
    u8 pad4[0x105 - 4];
    u8 flags;
    u8 pad106[0x128 - 0x106];
};
struct Group080A9 {
    u8 pad0[0x3d4];
    struct Entry080A9 entries[4];
    u8 pad874[0x8da - 0x874];
    s16 count;
};

static inline u8 TestMask(u8 flags, u32 mask)
{
    if (flags & mask)
        return TRUE;
    return FALSE;
}

void SpritePool_FlagEntries(struct Group080A9 *g)
{
    s32 i = 0;
    s16 *countPtr = &g->count;
    if (i < *countPtr) {
        u32 mask = 1;
        s16 *savedCountPtr = countPtr;
        struct Entry080A9 *e = g->entries;
        u8 *flagsPtr = &e->flags;
        u32 *fieldPtr = &e->field0;
        do {
            if (TestMask(*flagsPtr, mask))
                *fieldPtr = mask;
            flagsPtr += sizeof(struct Entry080A9);
            fieldPtr = (u32 *)((u8 *)fieldPtr + sizeof(struct Entry080A9));
            i++;
        } while (i < *savedCountPtr);
    }
}
