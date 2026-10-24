#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Only allocation differs: the outer `entry` gets r2 (ROM: r1, shared with `e`), so the
// 0x06010000 temporary takes r1 and a copy to `e` is added at the loop top.
struct Entry {
    u32 src;
    u16 size, count;
};
struct Dma {
    vu32 src, dest, control;
};
extern struct Entry gUnk_02039000[];
extern u32 gUnk_03004304;
void ObjGfx_LoadTiles(u32 start)
{
    u32 i;
    struct Entry *entry = gUnk_02039000;
    u32 dst = 0x06010000 + (start << 5);
    i = 0;
    if (i < gUnk_03004304) {
        u32 n = gUnk_03004304;
        do {
            struct Entry *e = entry;
            u32 src = e->src;
            s32 j;
            for (j = 0; j < e->count; j++) {
                struct Dma *dma = (struct Dma *)0x040000d4;
                dma->src = src;
                dma->dest = dst;
                dma->control = (e->size >> 2) | 0x84000000;
                (void)dma->control;
                src += 0x200;
                dst += e->size;
            }

            i++;
            entry++;
        } while (i < n);
    }
    gUnk_03004304 = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", ObjGfx_LoadTiles);
#endif
