#include "numeric_overlay.h"

u16 *NumericOverlay_GetTileMapPtr(u32 bg, u32 x, s32 y)
{
    struct BgMapInfo *t = &gUnk_03004C30[bg];
    u16 *p = t->map;
    p = p + x;
    return p + (t->width << 1) * y;
}

INCLUDE_ASM("asm/nonmatching", sub_0800F4EC);

void NumericOverlay_RefreshDisplay(struct NumericOverlay *s)
{
    s32 a, b, c;
    u32 z;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    z = 0;
    s->dirty = 0xf;
    sub_0800F4EC(s);
    s->dirty = z;
}

// Returns the (unchanged) r0 of NumericOverlay_RefreshDisplay.
extern u32
NumericOverlay_RefreshResult(struct NumericOverlay *) asm("NumericOverlay_RefreshDisplay");
u32 NumericOverlay_Update(struct NumericOverlay *p)
{
    return NumericOverlay_RefreshResult(p);
}

u32 NumericOverlay_Destroy(void)
{
    gUnk_02000048 = 0;
}

// The status screen initializer without its timers, fades and scanline state: load the map and
// palette, clear the first 15 map rows to tile 0xD000 and copy the palette to the work buffer.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Allocation differs throughout (p takes r5, not r6), and the ROM reloads gUnk_03004C30 in the inner loop's preheader where agbcc shares one register; the sibling status screen fill (sub_0800E974) is also a draft.
s32 sub_0800F6F8(struct NumericOverlay *p, u16 id)
{
    u8 *map;
    u8 *palette;
    u16 *tiles;
    u16 *buffer;
    int row;
    typeof(p->mapId) t1;
    gUnk_02000048 = p;
    p->id = id;
    p->flags = Script_GetKeywordValue('f', 0);
    p->mapId = (t1 = 0x5AF3);
    p->paletteId = Script_GetKeywordValue('p', 0);
    p->buffer = gUnk_03005160;
    p->bg = 0;
    map = ScriptProc_Start(0xC091, t1);
    if (!map)
        return -1;
    palette = ScriptProc_Start(0x92B3, p->paletteId);
    if (!palette)
        return -1;
    if ((map[0] == 'M') && (map[1] == 'P'))
        Bg_SetPriority(p->bg, 0);
    else {
        void *dst = Video_GetBackgroundMap(0);
        Bg_SetupLayer(p->bg, 0, map, 0, 0, 0, 0, dst);
    }
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    tiles = NumericOverlay_GetTileMapPtr(0, 0, 0);
    row = 0;
    for (; row < 15; row++) {
        int col;
        for (col = 0; col < (gUnk_03004C30[0].width * 2); col++)
            *(tiles++) = 0xD000;
    }

    buffer = p->buffer;
    CpuSet(p->source, buffer, 0x04000018);
    p->source = (u16 *)(palette + 0x1B4);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0800F6F8);
#endif

void *NumericOverlay_Create(u32 arg)
{
    void *obj;
    obj = Task_Create(9, 0x2C);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))NumericOverlay_Update,
                          (void (*)(void *))NumericOverlay_Destroy);
        if (sub_0800F6F8(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
