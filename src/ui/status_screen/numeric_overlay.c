#include "ui/status_screen/numeric_overlay.h"
#include "engine/video/render.h"

u16 *NumericOverlay_GetTileMapPtr(u32 bg, u32 x, s32 y)
{
    struct BackgroundLayer *layer = &gBgRecords[bg];
    u16 *map = layer->map;
    return map + x + (layer->width << 1) * y;
}

void sub_0800F4EC(struct NumericOverlay *unused)
{
    int digits[2];
    u16 *dst;
    int row, col, width, base;
    u32 hours, minutes;
    struct StatusGameState *clock = &gSunClock;
    if (!clock)
        return;
    if (clock->mode == 0 || clock->mode == 4 || clock->mode == 5) {
        dst = NumericOverlay_GetTileMapPtr(0, 4, 15);
        hours = clock->altCounters[0];
        minutes = clock->altCounters[1];
    } else {
        dst = NumericOverlay_GetTileMapPtr(0, 4, 15);
        hours = clock->counters[0];
        minutes = clock->counters[1];
    }
    for (row = 0; row < 2; row++) {
        base = 0x6a;
        for (col = 0; col < 4; col++)
            dst[col] = ((row << 4) + base + col) | 0xD000;
        dst += gBgRecords[0].width * 2;
    }

    dst = NumericOverlay_GetTileMapPtr(0, 8, 15);
    if (clock->mode == 0 || clock->mode == 4 || clock->mode == 5) {
        width = 2;
        base = 0x6e;
    } else {
        width = 2;
        base = 0x4e;
    }
    for (row = 0; row < 2; row++) {
        for (col = 0; col < width; col++)
            dst[col] = ((row << 4) + base + col) | 0xD000;
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(hours, 10);
    digits[1] = hours - digits[0] * 10;
    dst = NumericOverlay_GetTileMapPtr(0, 16, 15);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 0x60;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(minutes, 10);
    digits[1] = minutes - digits[0] * 10;
    dst = NumericOverlay_GetTileMapPtr(0, 22, 15);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 0x60;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }
}

void NumericOverlay_RefreshDisplay(struct NumericOverlay *s)
{
    s32 a, b, c;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    s->dirty = 0xf;
    sub_0800F4EC(s);
    s->dirty = 0;
}

u32 NumericOverlay_Update(struct NumericOverlay *p)
{
    NumericOverlay_RefreshDisplay(p);
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
    int row;
    gNumericOverlay = p;
    p->id = id;
    p->flags = Script_GetKeywordValue('f', 0);
    p->mapId = 0x5AF3;
    p->paletteId = Script_GetKeywordValue('p', 0);
    p->buffer = gBgPaletteBank13;
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
    for (row = 0; row < 15; row++) {
        int col = 0;
        if (col < gBgRecords[0].width * 2) {
            do {
                *tiles++ = 0xD000;
                col++;
            } while (col < gBgRecords[0].width * 2);
        }
    }

    p->source = (u16 *)(palette + 0x1B4);
    CpuCopy32(p->source, p->buffer, 0x60);
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
