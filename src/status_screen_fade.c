#include "palette_channel.h"
#include "status_screen.h"
#include "engine.h"
#include "numeric_overlay.h"
#include "ram.h"
#include "render.h"
#include "gba/io_reg.h"

void StatusScreen_FadeAndExit(struct StatusScreen *p)
{
    int factor = 32 - p->timer;
    int bank, i;
    int blueMask;
    u16 *buffer;

    bank = 0;
    buffer = p->buffer;
    blueMask = 0x7C00;
    for (; bank < 2; bank++) {
        for (i = 0; i < 16; i++) {
            int entry = bank * 16 + i;
            int color = buffer[entry];
            int r, g, b;
            int red, green, blue;

            PALETTE_EXTRACT_CHANNEL(red, color, 0x001F);
            r = (red * factor) & 0x7C0;
            PALETTE_EXTRACT_CHANNEL(green, color, 0x03E0);
            g = (green * factor) & 0xF800;
            PALETTE_EXTRACT_CHANNEL(blue, color, blueMask);
            b = (blue * factor) & 0x1F0000;
            buffer[entry] = (r | g | b) >> 6;
        }
    }

    if (++p->timer > 31) {
        Bg_ClearTilemap(0);
        Script_Call(p->script, &p->args);
        Task_Kill(p);
    } else {
        p->dirty |= 15;
    }
}

void StatusScreen_WaitForStart(struct StatusScreen *p)
{
    Bg_DisableDisplay(DISPCNT_BG0_ON);
    if (!StatusScreen_CheckConfirm(p)) {
        u16 old = p->delay--;
        if (old == 0) {
            Bg_EnableDisplay(DISPCNT_BG0_ON);
            StatusScreen_SetState(gStatusScreen, StatusScreen_Reveal);
        }
    }
}

void StatusScreen_FadeInPalette(struct StatusScreen *p)
{
    int factor = p->fade, red, green, blue, base, bank, i;
    int other;
    if (factor > 256) {
        factor = 256;
    }
    other = 256 - factor;
    PALETTE_EXTRACT_CHANNEL(red, p->blend, 31);
    PALETTE_EXTRACT_CHANNEL(green, p->blend, 992);
    PALETTE_EXTRACT_CHANNEL(blue, p->blend, 31744);
    for (base = 0, bank = 0; bank < 2;) {
        i = p->start;
        {
            u16 *end = &p->end;
            int nextBase = base + 16;
            int nextBank = bank + 1;
            for (; i < *end; i++) {
                u16 *source = &p->source[base + i], *dest = &p->buffer[base + i];
                int color = *source;
                int r;
                int g;
                int b;
                int sourceRed, sourceGreen;
                PALETTE_EXTRACT_CHANNEL(sourceRed, color, 31);
                r = (sourceRed * factor + other * red) & 0x1F00;
                PALETTE_EXTRACT_CHANNEL(sourceGreen, color, 992);
                g = (sourceGreen * factor + other * green) & 0x3E000;
                PALETTE_EXTRACT_CHANNEL(b, color, 31744);
                b = (b * factor + other * blue) & 0x7C0000;
                *dest = (r | g | b) >> 8;
            }
            base = nextBase;
            bank = nextBank;
        }
    }
    if (p->fade > 256) {
        p->effect = 0;
        p->fade = 0;
    }
    p->fade += p->fadeStep;
}

void StatusScreen_FadeOutPalette(struct StatusScreen *p)
{
    int other, red, green, blue, base, bank, i;
    int factor = p->fade;
    if (factor > 256)
        factor = 256;
    other = 256 - factor;
    red = p->blend & 31;
    green = p->blend & 992;
    blue = p->blend & 31744;
    for (base = 0, bank = 0; bank < 2; base += 16, bank++) {
        for (i = p->start; i < p->end; i++) {
            u16 *source = &p->source[base + i], *dest = &p->buffer[base + i];
            int color = *source;
            int r;
            int g;
            int b;
            r = (factor * red + (color & 31) * other) & 0x1F00;
            g = (factor * green + (color & 992) * other) & 0x3E000;
            b = 31744;
            b &= color;
            b = (factor * blue + b * other) & 0x7C0000;
            *dest = (r | g | b) >> 8;
        }
    }
    if (p->fade > 256) {
        p->effect = 0;
        p->fade = 0;
    }
    p->fade += p->fadeStep;
}

static inline int StatusScreen_Random(void)
{
    u16 offset;
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)((const u8 *)table + offset);
}
static inline void StatusScreen_AdvanceNoiseAmplitude(struct StatusScreen *p)
{
    s16 *amplitude = &p->amplitude;
    u16 *delta = &p->delta;
    *amplitude += *delta;
    if (*amplitude < 0)
        *amplitude = 0;
    else if (*amplitude > 256)
        *amplitude = 256;
}
void StatusScreen_UpdateScanlineNoise(struct StatusScreen *p)
{
    s32 i;
    for (i = 0; i < p->top; i++)
        p->rows[p->active][i] = 0;
    i = p->top;
    for (; i < p->bottom; i++) {
        p->rows[p->active][i] = (StatusScreen_Random() * p->amplitude) >> 18;
    }
    for (i = p->bottom; i < 160; i++)
        p->rows[p->active][i] = 0;
    gScanlineDmaSource = p->rows[p->active];
    gScanlineDmaTarget = (void *)0x0400004C;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 1;
    StatusScreen_AdvanceNoiseAmplitude(p);
}

void StatusScreen_ClearScanlineNoise(struct StatusScreen *p)
{
    CpuFill32(0, p->rows, 0x91 * sizeof(u32));
    gScanlineDmaSource = p->rows[p->active];
    gScanlineDmaTarget = (void *)0x0400004C;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 0;
    p->noise = 0;
}

int StatusScreen_Update(struct StatusScreen *p)
{
    if (p->state)
        p->state(p);
    if (p->effect)
        p->effect(p);
    if (p->noise)
        p->noise(p);
    {
        u16 next = 1 - p->active;
        p->active = next;
    }
    StatusScreen_RefreshDirtyDisplays(p);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
}

int StatusScreen_Destroy(struct StatusScreen *p)
{
    CpuFill32(0, p->rows, 0x91 * sizeof(u32));
    gScanlineDmaSource = p->rows[p->active];
    gScanlineDmaTarget = (void *)0x0400004C;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 0;
    gStatusScreen = 0;
}

int StatusScreen_Initialize(struct StatusScreen *p, u16 id)
{
    int i;
    u8 *map, *palette;
    gStatusScreen = p;
    Bg_DisableDisplay(DISPCNT_BG0_ON);
    p->id = id;
    p->flags = Script_GetKeywordValue('f', 0);
    p->mapId = 0x5AF3;
    p->paletteId = Script_GetKeywordValue('p', 0);
    p->buffer = gBgPaletteBank13;
    p->script = Script_GetKeywordValue('e', 0);
    if (Script_SeekToKeyword('a')) {
        p->args.values = p->values;
        for (i = 0; i < 8 && Script_GetPc(); i++)
            p->args.values[i] = Script_GetValue();
        p->args.count = i;
    } else
        p->args.count = 0;
    if (Script_SeekToKeyword('t')) {
        for (i = 0; i < 6 && Script_GetPc(); i++) {
            int v = Script_GetValue();
            p->times[i] = v;
            if ((u16)v < gUnk_08604B3C[i])
                p->times[i] = gUnk_08604B3C[i];
        }
        for (; i < 6; i++)
            p->times[i] = gUnk_08604B3C[i];
    } else
        for (i = 0; i < 6; i++)
            p->times[i] = gUnk_08604B3C[i];
    p->bg = 0;
    map = ScriptProc_Start(0xC091, p->mapId);
    if (!map)
        return -1;
    palette = ScriptProc_Start(0x92B3, p->paletteId);
    if (!palette)
        return -1;
    if (map[0] == 'M' && map[1] == 'P')
        Bg_SetPriority(p->bg, 0);
    else {
        void *dst = Video_GetBackgroundMap(0);
        Bg_SetupLayer(p->bg, 0, map, 0, 0, 0, 0, dst);
    }
    p->source = (u16 *)(palette + 0x1B4);
    CpuFill32(0, p->buffer, 3 * 16 * sizeof(*p->buffer));
    REG_BG0CNT |= BGCNT_MOSAIC;
    p->delay = Script_GetKeywordValue('w', 0);
    StatusScreen_SetState(p, StatusScreen_WaitForStart);
    p->start = 0;
    p->end = 16;
    p->blend = 0x1084;
    p->fade = 0;
    p->fadeStep = 1;
    p->effect = 0;
    CpuFill32(0, p->rows, 0x91 * sizeof(u32));
    p->active = 0;
    p->amplitude = 1024;
    p->delta = 1;
    p->top = 0;
    p->bottom = 160;
    p->noise = 0;
    p->dirty = 15;
    return 0;
}

void *StatusScreen_Open(u32 arg)
{
    void *obj;
    obj = Task_Create(9, 0x308);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))StatusScreen_Update,
                          (void (*)(void *))StatusScreen_Destroy);
        if (StatusScreen_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}

void NumericOverlay_ClearActive(void)
{
    gNumericOverlay = 0;
}
