#include "global.h"
#include "gba/gba.h"

// MATCHING_CONCESSION: register-pinning
// Upload queued tile slots to OBJ VRAM using DMA3.
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

void sub_0821537C(void)
{
    u32 dest = 0x06010000 + (gUnk_030042F4 * 32);
    struct ObjGfxSlot *slot = gUnk_03001148;
    s32 i;
    s32 n = gUnk_030042F0;
    if (n != 0) {
        vu32 *dmaRegs = (vu32 *)0x040000D4;
        u32 mode = 0x84000000;
        i = n;
        do {
            s32 bytes = slot->size << 5;
            dmaRegs[0] = (vu32)slot->src;
            dmaRegs[1] = (vu32)dest;
            dmaRegs[2] = (vu32)(mode | (bytes / 4));
            dmaRegs[2];
            dest += bytes;
            slot++;
        } while (--i != 0);
        asm(""
            :
            : "r"(&gUnk_030042F0), "r"(&gUnk_030042F0), "r"(mode), "r"(dmaRegs), "r"(dmaRegs),
              "r"(dmaRegs));
    }

    gUnk_030042F0 = 0;
    ObjGfx_LoadTiles(gUnk_030042F4 + gUnk_030042D8);
}
