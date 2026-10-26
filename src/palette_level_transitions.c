#include "palette_level_transitions.h"
#include "ram.h"

void Mosaic_SetParameters(u16, u8, u8);

void PaletteLevels_Increase(struct PaletteLevels *p)
{
    u16 levels = 0;
    int i;
    for (i = 0; i <= 3; i++) {
        levels |= p->current[i] << (i * 4);
        if ((p->enabled >> i) & 1) {
            if (++p->timers[i] >= p->periods[i]) {
                p->timers[i] = 0;
                if (p->current[i] >= p->target[i]) {
                    p->current[i] = p->target[i];
                    p->enabled &= ~(1 << i);
                } else
                    p->current[i]++;
            }
        }
    }
    Mosaic_SetParameters(levels, p->first, p->count);
    if (!p->enabled)
        p->state = 0;
}

void PaletteLevels_Decrease(struct PaletteLevels *p)
{
    u16 levels = 0;
    int i;
    for (i = 0; i <= 3; i++) {
        levels |= p->current[i] << (i * 4);
        if ((p->enabled >> i) & 1) {
            if (++p->timers[i] >= p->periods[i]) {
                p->timers[i] = 0;
                if (p->current[i] <= p->target[i]) {
                    p->current[i] = p->target[i];
                    p->enabled &= ~(1 << i);
                } else
                    p->current[i]--;
            }
        }
    }
    Mosaic_SetParameters(levels, p->first, p->count);
    if (!p->enabled)
        p->state = 0;
}

extern s32 (*gUnk_08604C58[])(struct PaletteLevels *);

s32 PaletteLevels_Update(struct PaletteLevels *p)
{
    s32 (*f)(struct PaletteLevels *) = gUnk_08604C58[p->state];
    if (f)
        f(p);
}

void Mosaic_ResetParameters(void);

u32 PaletteLevels_Destroy(void)
{
    Mosaic_ResetParameters();
    gPaletteLevels = 0;
    return 0;
}

s32 PaletteLevels_Initialize(struct PaletteLevels *s, u16 unused)
{
    s32 i;
    gPaletteLevels = s;
    s->state = 0;
    s->first = 0;
    s->count = 0;
    s->enabled = 0;
    for (i = 0; i < 4; i++) {
        s->timers[i] = 0;
        s->periods[i] = 0;
        s->current[i] = 0;
        s->target[i] = 0;
    }
}

struct PaletteLevels *PaletteLevels_Create(u32 a, u32 unused)
{
    struct PaletteLevels *p;
    if (gPaletteLevels != 0)
        return gPaletteLevels;
    p = Task_Create(9, 0x34);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))PaletteLevels_Update,
                          (void (*)(void *))PaletteLevels_Destroy);
        if (PaletteLevels_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

int PaletteLevels_Configure(int mode, int first, int count, u8 *initial, u8 *target, u16 *periods)
{
    struct PaletteLevels *initialManager = gPaletteLevels;
    struct PaletteLevels *p;
    int i;
    if (!initialManager) {
        p = PaletteLevels_Create(0, 0);
        if (!p)
            return -1;
    } else
        p = initialManager;
    p->state = mode;
    p->first = first;
    p->count = count;
    p->enabled = 15;
    for (i = 0; i <= 3; i++) {
        p->timers[i] = 0;
        p->periods[i] = periods[i];
        p->current[i] = initial[i] & 15;
        p->target[i] = target[i] & 15;
    }
    return 0;
}

/* Script: -m, -f, -t (default 0 each), -c and -a four bytes each and -i four
   halfwords (all default 0), passed to PaletteLevels_Configure (registry 3535). */

s32 PaletteLevels_ConfigureFromScript(void)
{
    u8 c[4];
    u8 a[4];
    u16 iv[4];
    s32 m, f, t, k;
    u8 *pa;
    u16 *pi;
    m = Script_GetKeywordValue('m', 0);
    f = Script_GetKeywordValue('f', 0);
    t = Script_GetKeywordValue('t', 0);
    if (Script_SeekToKeyword('c')) {
        k = 0;
        pa = a;
        pi = iv;
        for (; k <= 3; k++)
            c[k] = Script_GetValue();
    } else {
        s32 zero;
        u8 *out;
        pa = a;
        pi = iv;
        zero = 0;
        out = c + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)c);
    }
    if (Script_SeekToKeyword('a')) {
        for (k = 0; k <= 3; k++)
            pa[k] = Script_GetValue();
    } else {
        u8 *base = pa;
        s32 zero = 0;
        u8 *out = pa + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)base);
    }
    if (Script_SeekToKeyword('i')) {
        u16 *out = pi;
        for (k = 0; k <= 3; k++)
            *out++ = Script_GetValue();
    } else {
        u16 *base = pi;
        s32 zero = 0;
        u16 *out = pi + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)base);
    }
    return PaletteLevels_Configure(m, f, t, c, pa, pi);
}

void PaletteLevels_Reset(void)
{
    struct PaletteLevels *p = gPaletteLevels;
    int i;
    if (p) {
        Mosaic_ResetParameters();
        p->state = 0;
        p->first = 0;
        p->count = 0;
        p->enabled = 0;
        for (i = 0; i <= 3; i++) {
            p->timers[i] = 0;
            p->periods[i] = 0;
            p->current[i] = 0;
            p->target[i] = 0;
        }
    }
}

/* This range's trailing reset belongs to the next subsystem's manager. */
#define FloatingEffectManager_ClearActive sub_08016890
void FloatingEffectManager_ClearActive(void)
{
    gLinkedWidgetSet = 0;
}
