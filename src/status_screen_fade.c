#include "status_screen.h"
#include "engine.h"
#include "numeric_overlay.h"
#include "ram.h"
/* NumericOverlay_ClearActive: sets gNumericOverlay (the live NumericOverlay pointer, per numeric_overlay.h) back to NULL; twin of NumericOverlay_Destroy in numeric_overlay.c */
#define NumericOverlay_ClearActive sub_0800F4B8
// MATCHING_CONCESSION: register-pinning

void StatusScreen_FadeAndExit(struct StatusScreen *p)
{
    int factor = 32 - p->timer, bank, i;
    int blueMask;
    u16 *buffer;
    bank = 0;
    *(&buffer) = p->buffer;
    blueMask = 31744;
    for (; bank < 2; bank++) {
        for (i = 0; i < 16; i++) {
            {
                int index = i;
                i = index;
            }
            {
                u16 *out = &buffer[bank * 16 + i];
                int color = *out;
                int r = ((color & 31) * factor) & 0x7C0;
                int g = ((color & 992) * factor) & 0xF800;
                int blue = color & blueMask;
                DEBUG_NOP();
                {
                    int b = (blue * factor) & 0x1F0000;
                    *out = (r | g | b) >> 6;
                }
            }
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
    u32 mask = 256;
    gDispCntShadow &= ~mask;
    if (!StatusScreen_CheckConfirm(p)) {
        u32 old = p->delay;
        p->delay = old + 65535;
        asm("" : "+r"(old));
        if ((u16)old == 0) {
            {
                register int flags asm("r1") = gDispCntShadow;
                {
                    int result = mask;
                    gDispCntShadow = result | flags;
                }
            }
            {
                struct StatusScreen *target = gStatusScreen;
                {
                    register void (*state)(struct StatusScreen *) = StatusScreen_Reveal;
                    target->state = state;
                    target->timer = 0;
                }
            }
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
    red = p->blend & 31;
    green = p->blend & 992;
    blue = p->blend & 31744;
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
                do {
                    r = ((color & 31) * factor + other * red) & 0x1F00;
                    g = ((color & 992) * factor + other * green) & 0x3E000;
                    b = color & 31744;
                } while (0);
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
            register int color asm("r1") = *source;
            int r;
            int g;
            int b;
            r = (factor * red + (color & 31) * other) & 0x1F00;
            g = (factor * green + (color & 992) * other) & 0x3E000;
            b = (factor * blue + (color & 31744) * other) & 0x7C0000;
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
    u32 zero = 0;
    CpuSet(&zero, p->rows, 0x05000091);
    gScanlineDmaSource = p->rows[p->active];
    gScanlineDmaTarget = (void *)0x0400004C;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 0;
    p->noise = 0;
}

// The task callback returns the tilemap loader's r0 (engine.h declares it void).
extern int BgGfx_LoadTilemapResult(u32, u32, u32, u32, u32) asm("BgGfx_LoadTilemapSimple");
int StatusScreen_Update(struct StatusScreen *p)
{
    int zero;
    if (p->state)
        p->state(p);
    if (p->effect)
        p->effect(p);
    if (p->noise)
        p->noise(p);
    {
        u16 next = 1 - p->active;
        zero = 0;
        p->active = next;
    }
    StatusScreen_RefreshDirtyDisplays(p);
    return BgGfx_LoadTilemapResult(0, 0, 0, 0, zero);
}

// This callback preserves the BIOS wrapper's r0 result on return.
extern int CpuSetResult(const void *, void *, u32) asm("CpuSet");
int StatusScreen_Destroy(struct StatusScreen *p)
{
    u32 zero = 0;
    int result = CpuSetResult(&zero, p->rows, 0x05000091);
    gScanlineDmaSource = p->rows[p->active];
    gScanlineDmaTarget = (void *)0x0400004C;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 0;
    gStatusScreen = 0;
    return result;
}

int StatusScreen_Initialize(struct StatusScreen *p, u16 id)
{
    int i;
    u8 *map, *palette;
    gStatusScreen = p;
    {
        u16 *flags = &gDispCntShadow;
        {
            int mask = ~256;
            mask &= *flags;
            *flags = mask;
        }
    }
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
    {
        u32 zero = 0;
        CpuSet(&zero, p->buffer, 0x05000018);
    }
    *(volatile u16 *)0x04000008 |= 0x40;
    p->delay = Script_GetKeywordValue('w', 0);
    {
        register void (*state)(struct StatusScreen *) = StatusScreen_WaitForStart;
        p->state = state;
    }
    p->timer = 0;
    p->start = 0;
    p->end = 16;
    p->blend = 0x1084;
    p->fade = 0;
    p->fadeStep = 1;
    p->effect = 0;
    {
        u32 zero = 0;
        CpuSet(&zero, p->rows, 0x05000091);
    }
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

void sub_0800F4B8(void)
{
    gNumericOverlay = 0;
}
