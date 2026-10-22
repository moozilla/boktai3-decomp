#include "global.h"

struct BgResource {
    u8 pad[0xC];
    u32 maps;
    u32 tiles;
    u32 extra;
};
struct BgEnt {
    u16 id;
    u16 count;
    u32 off;
};
struct Dma {
    vu32 src, dst, ctl;
};
struct BgEnt *BgGfx_FindEntryById(struct BgResource *h, u16 id);

s32 sub_08215D40(s32 layer, struct BgResource *res, s32 count, u16 *ids, s32 start)
{
    struct BgResource hdr;
    struct BgEnt *ent;
    u16 *p;
    s32 i, j;
    u32 dst, off;

    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    off = start << 5;
    if (layer == 0)
        off = 0x8000;
    dst = off;
    for (i = 0; i < count; i++) {
        ent = BgGfx_FindEntryById(&hdr, ids[i]);
        p = (u16 *)(ent->off + hdr.tiles);
        for (j = 0; j < ent->count; j++) {
            u32 s = hdr.extra + (*p << 5);
            u32 t = dst + 0x06000000;
            {
                struct Dma *d = (struct Dma *)0x040000D4;
                d->src = s;
                d->dst = t;
                d->ctl = 0x84000008;
                d->ctl;
            }
            p++;
            dst += 0x20;
        }
    }
    return 0;
}
