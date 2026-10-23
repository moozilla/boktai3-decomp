#include "global.h"
#include "gba/gba.h"

#ifdef NONMATCHING
// fuzzy: 78.1%
// fix.py best version (reprinted C)
// ObjGfx: DMA queued tile slots to OBJ VRAM; ROM keeps a separate loop counter and a copy before the asrs.
struct ObjGfxSlot {
    u16 size;
    u16 offset;
    const void *src;
};
extern struct ObjGfxSlot gUnk_03001148[];
extern s16 gUnk_030042D8;
extern u16 gUnk_030042F0;
extern s16 gUnk_030042F4;
void ObjGfx_LoadTiles(u32 start);

static inline u32 Arg1(u32 k_, s8 bytes)
{
    return bytes >> k_;
}

void sub_0821537C(void)
{
    u32 dest = 0x06010000 + (gUnk_030042F4 * 32);
    struct ObjGfxSlot *slot = gUnk_03001148;
    s32 i;
    s32 n = gUnk_030042F0;
    for (i = n; i != 0; i--) {
        s8 bytes = slot->size * 32;
        {
            vu32 *dmaRegs = (vu32 *)(0x4000000 + 0xd4);
            dmaRegs[0] = (vu32)slot->src;
            dmaRegs[1] = (vu32)dest;
            dmaRegs[2] = (vu32)(0x84000000 | Arg1(2, bytes));
            dmaRegs[2];
        };
        dest += bytes;
        slot++;
    }

    gUnk_030042F0 = 0;
    ObjGfx_LoadTiles(gUnk_030042F4 + gUnk_030042D8);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821537C);
#endif
