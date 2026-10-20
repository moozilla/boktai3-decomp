#include "global.h"

struct BgResource {
    u8 pad[0xC];
    u32 maps;
    u32 tiles;
    u32 extra;
};
struct BgEnt {
    u16 id;
    u16 pad;
    u32 off;
};
struct BgEnt *BgGfx_FindEntryById(struct BgResource *h, u16 id);
void CpuSet(const void *src, void *dst, u32 ctl);

s32 Bg_CopyTiles(struct BgResource *res, u32 id, u8 *dst, s32 start, u32 count)
{
    struct BgResource hdr;
    struct BgEnt *ent;
    u16 *p;
    s32 off;
    u32 i;
    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    ent = BgGfx_FindEntryById(&hdr, (u16)id);
    if (ent == 0)
        return -1;
    off = 0;
    p = (u16 *)(ent->off + hdr.tiles);
    p += start;
    for (i = 0; i < count; i++) {
        CpuSet((u8 *)hdr.extra + (*p << 5), dst + off, 0x04000008);
        p++;
        off += 0x20;
    }
    return 0;
}
