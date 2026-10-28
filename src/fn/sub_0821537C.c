#include "global.h"
#include "gba/gba.h"
#include "render.h"

// MATCHING_CONCESSION: register-pinning
// Upload queued tile slots to OBJ VRAM using DMA3.
void ObjGfx_LoadTiles(u32 start);

void sub_0821537C(void)
{
    u32 dest = 0x06010000 + (((s16)gStaticTileCount) * 32);
    struct ObjGfxSlot *slot = gObjGfxSlots;
    s32 i;
    s32 n = gQueuedObjSlotCount;
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
            : "r"(&gQueuedObjSlotCount), "r"(&gQueuedObjSlotCount), "r"(mode), "r"(dmaRegs),
              "r"(dmaRegs), "r"(dmaRegs));
    }

    gQueuedObjSlotCount = 0;
    ObjGfx_LoadTiles(((s16)gStaticTileCount) + ((s16)gDynamicTileSum));
}
