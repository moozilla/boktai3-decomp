#include "global.h"
/* Tentative task family name; entries have a 0x1ac-byte stride. */
struct Task4E01Entry {
    u8 pad0[0x18];
    u16 flags;
    u8 pad1A[0x46];
    s32 kind;
    u8 pad64[8];
    u8 *owner;
    u8 pad70[0x134];
    s16 value1A4;
    u8 pad1A6[6];
};
struct Task4E01 {
    u8 pad0[0x18];
    u32 activeMask;
    u8 pad1C[0x20];
    s32 count;
    u8 pad40[0xc8];
    struct Task4E01Entry *entries;
};
struct Task4E01 *ResourceRegistry_GetResource(u16);
