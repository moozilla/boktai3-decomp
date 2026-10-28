#include "numeric_overlay.h"
#include "render.h"

u16 *NumericOverlay_GetTileMapPtr(u32 bg, u32 x, s32 y)
{
    struct BackgroundLayer *t = &gBgRecords[bg];
    u16 *p = t->map;
    p = p + x;
    return p + (t->width << 1) * y;
}

// MATCHING_CONCESSION: register-pinning
void sub_0800F4EC(struct NumericOverlay *unused)
{
    int digits[2];
    s16 *dst;
    int row, col, n, base;
    register int *values asm("ip");
    u32 hours, minutes;
    struct StatusGameState *c = &gUnk_03005430;
    if (!c)
        return;
    if (c->mode == 0 || c->mode == 4 || c->mode == 5) {
        dst = (s16 *)NumericOverlay_GetTileMapPtr(0, 4, 15);
        hours = c->altCounters[0];
        minutes = c->altCounters[1];
    } else {
        dst = (s16 *)NumericOverlay_GetTileMapPtr(0, 4, 15);
        hours = c->counters[0];
        minutes = c->counters[1];
    }
    for (row = 0; row < 2; row++) {
        int shift = row << 4;
        base = 0x6a;
        for (col = 0; col < 4; col++)
            dst[col] = (shift + base + col) | -12288;
        dst += gBgRecords[0].width * 2;
    }

    dst = (s16 *)NumericOverlay_GetTileMapPtr(0, 8, 15);
    if (c->mode == 0 || c->mode == 4 || c->mode == 5) {
        n = 2;
        base = 0x6e;
    } else {
        n = 2;
        base = 0x4e;
    }
    for (row = 0; row < 2; row++) {
        int shift = row << 4;
        for (col = 0; col < n; col++)
            dst[col] = (shift + base + col) | -12288;
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(hours, 10);
    digits[1] = hours - digits[0] * 10;
    dst = (s16 *)NumericOverlay_GetTileMapPtr(0, 16, 15);
    row = 0;
    values = digits;
    for (; row < 2; row++) {
        int shift = row << 4;
        int *digit = values;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = *digit++ + 0x60;
            *out++ = shift + tile | -12288;
        }
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(minutes, 10);
    digits[1] = minutes - digits[0] * 10;
    dst = (s16 *)NumericOverlay_GetTileMapPtr(0, 22, 15);
    row = 0;
    values = digits;
    for (; row < 2;) {
        int next = row + 1;
        int shift = row << 4;
        int *digit = values;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = *digit++ + 0x60;
            *out++ = shift + tile | -12288;
        }
        dst += gBgRecords[0].width * 2;
        row = next;
    }
}

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
    gNumericOverlay = 0;
}

// The status screen initializer without its timers, fades and scanline state: load the map and
// palette, clear the first 15 map rows to tile 0xD000 and copy the palette to the work buffer.
s32 sub_0800F6F8(struct NumericOverlay *p, u16 id)
{
    u8 *map;
    u8 *palette;
    u16 *tiles;
    u16 *buffer;
    int row;
    gNumericOverlay = p;
    p->id = id;
    p->flags = Script_GetKeywordValue('f', 0);
    p->mapId = 0x5AF3;
    p->paletteId = Script_GetKeywordValue('p', 0);
    p->buffer = gUnk_03005160;
    p->bg = 0;
    map = ScriptProc_Start(0xC091, p->mapId);
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
        col = 0;
        if (col < gBgRecords[0].width * 2) {
            do {
                *tiles++ = 0xD000;
                col++;
            } while (col < gBgRecords[0].width * 2);
        }
    }

    buffer = p->buffer;
    p->source = (u16 *)(palette + 0x1B4);
    CpuSet(p->source, buffer, 0x04000018);
    return 0;
}

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
