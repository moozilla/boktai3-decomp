#include "global.h"

struct Obj {
    u8 pad0[0x18];
    s16 id;
    u8 kw;
    u8 bg;
    u16 mapId;
    u16 palId;
    u32 *buffer;
    u16 *source;
};

struct Gfx {
    u8 pad[0x18];
    s16 w;
};

extern struct Obj *gUnk_02000048;
extern struct Gfx gUnk_03004C30;
extern u32 gUnk_03005160[];

void BgGfx_LoadTilemapSimple(u32 a, u32 b, u32 c, u32 d, u32 e);
void CpuSet(const void *src, void *dest, u32 control);
void *ScriptProc_Start(u16 key, u16 context);
s32 Script_GetKeywordValue(u32, s32);
void *Video_GetBackgroundMap(u32);
u16 *sub_0800F4C4(u32 bg, u32 x, s32 y);
void sub_08215A74(u32, s32);
void sub_082160B8(u32, u32, void *, s32, s32, s32, s32, void *);

s32 sub_0800F6F8(struct Obj *p, u16 id)
{
    u8 *map;
    u8 *palette;
    u16 *dst;
    s32 y;
    u32 *buf;

    gUnk_02000048 = p;
    y = 0;
    p->id = id;
    p->kw = Script_GetKeywordValue('f', 0);
    p->mapId = 0x5AF3;
    p->palId = Script_GetKeywordValue('p', 0);
    p->buffer = gUnk_03005160;
    p->bg = y;
    map = ScriptProc_Start(0xC091, p->mapId);
    if (!map)
        return -1;
    palette = ScriptProc_Start(0x92B3, p->palId);
    if (!palette)
        return -1;
    if (map[0] == 'M' && map[1] == 'P')
        sub_08215A74(p->bg, 0);
    else {
        void *d = Video_GetBackgroundMap(0);
        sub_082160B8(p->bg, 0, map, 0, 0, 0, 0, d);
    }
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    dst = sub_0800F4C4(0, 0, 0);
    y = 0;
    buf = p->buffer;
    do {
        s32 x = 0;
        s32 next = y + 1;
        struct Gfx *g = &gUnk_03004C30;
        while (x < g->w * 2) {
            *dst++ = 0xD000;
            x++;
            g = &gUnk_03004C30;
        }
        y = next;
    } while (y <= 14);
    p->source = (u16 *)(palette + 0x1B4);
    CpuSet(p->source, buf, 0x04000018);
    return 0;
}
