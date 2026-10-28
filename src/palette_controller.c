#include "palette_controller.h"
#include "render.h"
/* Palette_ClearActiveController: zeroes gPaletteController (the active controller pointer); identical body to the task's destroy callback Palette_Destroy */
#define Palette_ClearActiveController sub_08002AD4
/* Palette_NoOpCallback: empty stub with no identified caller or callee; placeholder callback body */
#define Palette_NoOpCallback sub_08002E7C
/* Palette_SetBlendFromScript: reads the 'r' script keyword and forwards it to Palette_SetBlend */
#define Palette_SetBlendFromScript sub_08003494

void Palette_SetPendingFlags(struct PaletteController *p, u32 x)
{
    p->pending |= x;
}

void Palette_ClearPendingFlags(struct PaletteController *p, u32 x)
{
    p->pending &= ~x;
}

u32 Palette_TestPendingFlags(struct PaletteController *p, u32 x)
{
    return p->pending & x;
}

u16 *Palette_GetColorSet(struct PaletteController *p, u32 set, u32 slot)
{
    return p->colorSets[set][slot];
}

void sub_08002AD4(void)
{
    gPaletteController = 0;
}

static inline void copy32(const void *source, void *dest, s32 size)
{
    CpuSet(source, dest, (size / 4 & 0x1fffff) | 0x04000000);
}

void Palette_BlendRange(u16 *dest, const u16 *source, const u16 *target, u32 amount, s32 first,
                        s32 end)
{
    s32 k2 = 0x7C1F;
    s32 count;
    u32 inverse;
    amount >>= 1;
    inverse = 32 - amount;
    count = end - first;
    if (count) {
        if (amount >= 32) {
            const u16 *to = target + first;
            dest += first;
            copy32(to, dest, count * 2);
        } else {
            const u16 *a = source + first, *b = target + first;
            u16 *out;
            s32 i;
            dest += first;
            out = dest;
            if (count > 0) {
                u32 greenMask = 0x03E0;
                i = count;
                do {
                    u32 from = *a;
                    u32 fromRB;
                    u32 fromG;
                    u32 to;
                    fromRB = k2 & from;
                    do {
                        fromG = greenMask & from;
                    } while (0);
                    to = *b;
                    {
                        u32 rb = (((to & 0x7C1F) * amount) + (inverse * fromRB)) & 0xF83E0;
                        u32 g = (((to & greenMask) * amount) + (inverse * fromG)) & 0x7C00;
                        *out = (rb | g) >> 5;
                    }
                    a++;
                    b++;
                    out++;
                } while (--i);
            }
        }
    }
}

void Palette_ScaleRange(u16 *dest, const u16 *source, u32 brightness, s32 first, s32 end)
{
    s32 count;
    brightness >>= 1;
    count = end - first;
    if (count) {
        if (brightness >= 32) {
            u16 *to;
            source += first;
            to = dest + first;
            copy32(source, to, count * 2);
        } else {
            const u16 *in = source + first;
            u16 *out;
            s32 i;
            dest += first;
            out = dest;
            if (count > 0) {
                u32 rbMask = 0x7C1F, rbProduct = 0xF83E0;
                i = count;
                do {
                    u32 color = *in;
                    u32 rb = ((color & rbMask) * brightness) & rbProduct;
                    u32 g = ((color & 0x03E0) * brightness) & 0x7C00;
                    *out = (rb | g) >> 5;
                    in++;
                    out++;
                } while (--i);
            }
        }
    }
}

void Palette_GetStateBlendIndices(u16 *, u16 *);

void Palette_PickTransitionColors(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 x[2];
    u16 *y = &x[1];
    Palette_GetStateBlendIndices(x, y);
    t->from = p->colorSets[PALETTE_SET_VARIANT][x[0]];
    t->to = p->colorSets[PALETTE_SET_VARIANT][*y];
}

s32 Palette_GetBrightnessThreshold(void)
{
    u8 v = gSunClock.state - 1;
    if (v <= 2) {
        return 0x20;
    }
    return 0x40;
}

void Palette_BrightnessIdle(struct PaletteTransition *t)
{
    u32 z = 0;
    t->brightness = z;
    if (t->callback != 0) {
        Script_Call(t->callback, 0);
        t->callback = z;
    }
}

void Palette_BrightnessFadeIn(struct PaletteTransition *t)
{
    s32 current;
    current = t->brightness;
    if (current < Palette_GetBrightnessThreshold()) {
        t->brightness += 4;
        return;
    }
    t->brightness = Palette_GetBrightnessThreshold();
    t->mode = 2;
}

void Palette_BrightnessHold(struct PaletteTransition *t)
{
    s32 d;
    s32 a;
    u32 v;
    u32 *q;
    u32 u;
    s32 f1 = Palette_GetBrightnessThreshold();
    u = t->brightness;
    d = u - f1;
    if (d < 0)
        d = -d;
    if ((s32)u < Palette_GetBrightnessThreshold()) {
        a = d;
        if (a > 8)
            a = 8;
        v = t->brightness + a;
    } else {
        u = t->brightness;
        if ((s32)u > Palette_GetBrightnessThreshold()) {
            a = d;
            if (a > 8)
                a = 8;
            v = t->brightness - a;
        } else {
            v = Palette_GetBrightnessThreshold();
        }
    }
    t->brightness = v;
    q = &t->callback;
    if (*q != 0) {
        Script_Call(*q, 0);
        *q = 0;
    }
}

void Palette_BrightnessFadeOut(struct PaletteTransition *t)
{
    if (t->brightness > 1) {
        t->brightness -= 2;
    } else {
        t->brightness = 0;
        t->mode = 0;
    }
}

void Palette_BlendTransitionColors(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 *buf = p->blendBuffer;
    u16 *b = t->from;
    u16 *c = t->to;
    Palette_BlendRange(buf, b, c, t->level, 0, 0xd0);
    p->current = buf;
    p->dirty = 1;
}

void Palette_BlendOverlay(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 *r;
    u16 *d;
    if (p->flags & 1) {
        r = p->blendBuffer;
    } else if (p->flags & 8) {
        r = p->palettes[p->selected];
    } else {
        r = Palette_GetColorSet(p, 0, p->selected);
    }
    d = p->effectBuffer;
    Palette_BlendRange(d, r, t->colors, t->brightness, 0, 0xD0);
    p->current = d;
    p->dirty = 1;
}

void Palette_BuildOverlayColors(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 *r = Palette_GetColorSet(p, 0, 2);
    Palette_ScaleRange(t->colors, r, 0x32, 0, 0xD0);
}

void Palette_AdvanceTransitionBlend(struct PaletteController *p, struct PaletteTransition *t)
{
    Palette_BlendTransitionColors(p, t);
    t->level++;
    if (t->level > 0x3f)
        t->level = 0x40;
}

void Palette_ApplyOverlayBlend(struct PaletteController *p, struct PaletteTransition *t)
{
    Palette_BlendOverlay(p, t);
}

void Palette_ResetTransition(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    Palette_PickTransitionColors(p, t);
    t->level = 0x40;
    Palette_BuildOverlayColors(p, t);
    Palette_BlendTransitionColors(p, t);
}

void sub_08002E7C(void)
{
}

void Palette_UpdateOscillation(struct PaletteController *p)
{
    struct PaletteWave *w = &p->wave;
    if (w->delay == 0) {
        if (++w->timer >= w->period) {
            w->timer = 0;
            w->value += w->step;
            if (w->value <= 0) {
                w->value = 0;
                w->step = -w->step;
                w->delay = w->lowDelay;
            } else if (w->value >= 64) {
                w->value = 64;
                w->step = -w->step;
                w->delay = w->highDelay;
            }
            w->dirty = 1;
            w->phase = 0;
        }
    } else
        w->delay--;
    p->blend = w->value;
}

s32 Div(s32, s32);

void Palette_UpdateBlendSlide(struct PaletteController *p)
{
    struct PaletteSlide *s = &p->slide;
    s32 r;
    s32 time = s->time;
    if (time == 0) {
        r = s->start;
    } else {
        s32 duration = s->duration;
        if (duration == 0 || time >= duration) {
            p->slide.value = s->target;
            goto end;
        }
        r = Div(s->start * (duration - time) + time * s->target, duration);
    }
    p->slide.value = r;
    s->time++;
end:
    p->blend = s->value;
}

void Palette_InitBlendSlide(struct PaletteController *p)
{
    struct PaletteSlide *s = &p->slide;
    s->start = 0x40;
    s->target = 0x40;
    s->value = 0x40;
    s->time = 0;
    s->duration = 0;
}

void Palette_BlendVariants(struct PaletteController *p)
{
    u16 *out_3;
    u16 *out_2;
    s32 i;
    u16 *out;
    if (p->blend == 0) {
        i = 0;
        out = p->palettes[0];
        do {
            if (p->colorSets[PALETTE_SET_SOURCE][i]) {
                CpuSet(p->colorSets[PALETTE_SET_SOURCE][i], out, 0x04000080);
            }
            out += 256;
            i++;
        } while (i <= 3);
    } else if (p->blend == 64) {
        i = 0;
        out_3 = p->palettes[0];
        do {
            if (p->colorSets[PALETTE_SET_DESTINATION][i]) {
                CpuSet(p->colorSets[PALETTE_SET_DESTINATION][i], out_3, 0x04000080);
            }
            out_3 += 256;
            i++;
        } while (i <= 3);
    } else {
        i = 0;
        out_2 = p->palettes[0];
        do {
            if (p->colorSets[PALETTE_SET_SOURCE][i] && p->colorSets[PALETTE_SET_DESTINATION][i]) {
                Palette_BlendRange(out_2, p->colorSets[PALETTE_SET_SOURCE][i],
                                   p->colorSets[PALETTE_SET_DESTINATION][i], p->blend, 0, 208);
            }
            out_2 += 256;
            i++;
        } while (i <= 3);
    }
    p->current = p->palettes[p->selected];
    p->dirty = 1;
}

void Palette_SetVariantsFromSource(struct PaletteController *p)
{
    u16 **q = p->colorSets[PALETTE_SET_SOURCE];
    s32 i = 3;
    do {
        q[4 * PALETTE_SET_VARIANT] = *q; // copy source slot to the matching variant slot
        q++;
        i--;
    } while (i >= 0);
}

void Palette_SetVariantsFromPalettes(struct PaletteController *p)
{
    s32 i = 3;
    u16 *v = p->palettes[3];
    u16 **t = &p->colorSets[PALETTE_SET_VARIANT][3];
    do {
        *t = v;
        v -= 256;
        t--;
        i--;
    } while (i >= 0);
    Palette_PickTransitionColors(p, &p->transition);
}

s32 Weather_IsActive(void);

extern void *gUnk_0860499C[];

void Palette_ApplyFrame(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    if (p->flags & 8) {
        if (p->flags & 2)
            Palette_UpdateOscillation(p);
        else
            Palette_UpdateBlendSlide(p);
        if (p->blend != p->previousBlend)
            Palette_SetPendingFlags(p, 7);
    }
    if (p->flags & 1) {
        if (gSunClock.state != p->previousState) {
            Palette_PickTransitionColors(p, t);
            t->level = 0;
        }
        if (t->level <= 63)
            Palette_SetPendingFlags(p, 6);
    } else
        Palette_ClearPendingFlags(p, 2);
    if (Weather_IsActive() || (p->flags & 16)) {
        Callback_InvokeActor(t, gUnk_0860499C[t->mode]);
        if (t->brightness != t->previousBrightness)
            Palette_SetPendingFlags(p, 4);
    } else
        Palette_ClearPendingFlags(p, 4);
    if (Palette_TestPendingFlags(p, 1))
        Palette_BlendVariants(p);
    if (Palette_TestPendingFlags(p, 2))
        Palette_AdvanceTransitionBlend(p, t);
    if (Palette_TestPendingFlags(p, 4))
        Palette_ApplyOverlayBlend(p, t);
    if (p->current) {
        if (p->flags & 32)
            CpuSet(p->current, p->hardware, 0x04000060);
        else
            CpuSet(p->current, p->hardware, 0x04000068);
    }
}

s32 Palette_Update(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    p->current = 0;
    p->pending = 0;
    p->dirty = 0;
    Palette_ApplyFrame(p);
    p->previousBlend = p->blend;
    t->previousBrightness = t->brightness;
    p->previousState = gSunClock.state;
}

u32 Palette_Destroy(void)
{
    gPaletteController = 0;
}

static inline u16 blendEnabled(struct PaletteController *p)
{
    return p->flags & 8;
}

s32 Palette_InitializeFromScript(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    struct PaletteWave *w = &p->wave;
    do {
        gPaletteController = p;
        p->pending = 0;
        t->callback = 0;
        p->hardware = gBgPaletteBuffer;
        gBgFadeLevel = 64;
        gBgFadeApplied = 64;
        gBgFadeColor = 0x1084;
        gBgFadeBankMask = 0;
        p->flags = Script_GetKeywordValue('f', 0);
        if (Script_SeekToKeyword('p')) {
            s32 i = 0;
            u16 **out = p->colorSets[PALETTE_SET_SOURCE];
            while (Script_GetPc() && i <= 3) {
                s32 value = Script_GetValue();
                if (value != 0x30) {
                    *out = (u16 *)(ScriptProc_Start(0x92B3, value) + 20);
                } else {
                    *out = 0;
                }
                out++;
                i++;
            }
        }
        if (Script_SeekToKeyword('d')) {
            s32 i = 0;
            u16 **out = p->colorSets[PALETTE_SET_DESTINATION];
            while (Script_GetPc() && i <= 3) {
                s32 value = Script_GetValue();
                if (value != 0x30) {
                    *out = (u16 *)(ScriptProc_Start(0x92B3, value) + 20);
                } else {
                    *out = 0;
                }
                out++;
                i++;
            }
        }
    } while (0);
    if (p->flags & 8) {
        Palette_SetVariantsFromPalettes(p);
    } else {
        Palette_SetVariantsFromSource(p);
    }
    p->blend = 0;
    p->selected = Script_GetKeywordValue('c', 0);
    p->current = p->colorSets[PALETTE_SET_SOURCE][p->selected];
    Palette_InitBlendSlide(p);
    if (p->flags & 2) {
        p->flags |= 8;
        if (Script_SeekToKeyword('a')) {
            w->period = Script_GetValue();
            w->step = Script_GetValue();
            w->unused9 = Script_GetValue();
            w->unusedC = Script_GetValue();
            w->lowDelay = Script_GetValue();
            w->highDelay = Script_GetValue();
        } else {
            w->period = 4;
            w->step = 2;
            w->unused9 = 8;
            w->unusedC = 0xFFFF;
            w->lowDelay = 32;
            w->highDelay = 0;
        }
        w->timer = 0;
        w->value = 0;
        w->phase = 0;
    }
    if (p->flags & 1) {
        Palette_ResetTransition(p);
    }
    if (blendEnabled(p) && (p->flags & 2)) {
        Palette_BlendVariants(p);
    }
    if (p->flags & 1) {
        Palette_BlendTransitionColors(p, t);
    }
    if (p->current) {
        CpuSet(p->current, p->hardware, 0x04000068);
    }
    p->previousState = gSunClock.state;
    t->mode = 0;
    return 0;
}

struct PaletteController *Palette_GetOrCreate(void)
{
    struct PaletteController *r;
    if (gPaletteController != 0)
        return gPaletteController;
    r = Task_Create(11, 0xe94);
    if (r) {
        Task_SetCallbacks(r, Palette_Update, Palette_Destroy);
        if (Palette_InitializeFromScript(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

void Palette_SetBrightnessMode(u32 v)
{
    if (gPaletteController) {
        struct PaletteTransition *t = &gPaletteController->transition;
        t->mode = v;
    }
}

u8 Palette_IsDirty(void)
{
    if (gPaletteController == 0)
        return 0;
    return gPaletteController->dirty;
}

void Palette_SetBlend(u32 v)
{
    if (gPaletteController) {
        gPaletteController->blend = v;
    }
}

void sub_08003494(void)
{
    Palette_SetBlend(Script_GetKeywordValue('r', 0));
}

void Palette_ReplaceResource(u32 bank, u32 index, u32 resource, u32 flags)
{
    struct PaletteController *p = gPaletteController;
    if (p) {
        u8 *colors;
        u16 id = resource;
        colors = ScriptProc_Start(0x92B3, id) + 20;
        p->colorSets[bank][index] = (u16 *)colors;
        if (flags & 1) {
            if (p->flags & 1) {
                u16 first, second;
                Palette_GetStateBlendIndices(&first, &second);
                if (first == 2)
                    Palette_BuildOverlayColors(p, &p->transition);
                Palette_ResetTransition(p);
            }
            if (p->flags & 8)
                Palette_SetVariantsFromPalettes(p);
            else
                Palette_SetVariantsFromSource(p);
            p->previousBlend = -1;
            p->previousState = -1;
        }
    }
}

void Palette_ReplaceResourceFromScript(void)
{
    if (gPaletteController != 0) {
        s32 a = Script_GetKeywordValue(0x73, 0);
        s32 b = Script_GetKeywordValue(0x69, 0);
        s32 c = Script_GetKeywordValue(0x6e, 0);
        s32 d = Script_GetKeywordValue(0x66, 0);
        Palette_ReplaceResource(a, b, c, d);
    }
}

void Palette_StartSlide(u32 a, u32 b, u32 c)
{
    struct PaletteController *p = gPaletteController;
    if (p != 0) {
        struct PaletteSlide *q = &p->slide;
        q->start = a;
        q->target = b;
        p->slide.value = a;
        q->time = 0;
        q->duration = c;
        p->flags |= 0xc;
        Palette_SetVariantsFromPalettes(p);
    }
}

void Palette_StartSlideFromScript(void)
{
    s32 a = Script_GetKeywordValue(0x73, 0);
    s32 b = Script_GetKeywordValue(0x61, 0);
    s32 c = Script_GetKeywordValue(0x74, 0);
    Palette_StartSlide(a, b, c);
}

void Palette_SetBrightnessFromScript(void)
{
    struct PaletteController *p = gPaletteController;
    if (p) {
        struct PaletteTransition *t = ({
            struct PaletteTransition *hint = &p->transition;
            hint;
        });
        s32 enabled = Script_GetKeywordValue('n', 0);
        u32 query = Script_SeekToKeyword('q');
        u32 immediate = ((0u - query) | query) >> 31;
        s32 callback = Script_GetKeywordValue('p', 0);
        u32 mode;
        p->transition.callback = 0;
        if (!(p->flags & 16)) {
            p->flags |= 16;
            if (t->mode == 0) {
                t->brightness = 0;
                t->previousBrightness = 0;
            }
        }
        p->flags |= 16;
        if (enabled) {
            if (immediate) {
                t->mode = 2;
                t->brightness = Palette_GetBrightnessThreshold();
                goto complete;
            }
            if (t->mode == 1) {
                goto waiting;
            }
            if (t->mode == 2) {
                goto complete;
            }
            mode = 1;
            goto setMode;
        } else {
            if (!immediate) {
                if (t->mode == 0) {
                    goto complete;
                }
                if (t->mode != 3) {
                    goto fadeOut;
                }
                goto waiting;
            }
            t->mode = 0;
            t->brightness = 0;
            p->flags &= ~16;
        }
    complete:
        if (callback) {
            Script_Call(callback, 0);
        }

        return;
    fadeOut:
        mode = 3;

        do {
        setMode:
            t->mode = mode;
        waiting:
            t->callback = callback;
        } while (0);
    }
}

u32 Palette_GetTransitionLevel(void)
{
    return gPaletteController ? gPaletteController->transition.level : 0x40;
}

u32 Palette_GetBrightness(void)
{
    struct PaletteController *q = gPaletteController;
    u32 r;
    if (q) {
        struct PaletteTransition *t = &q->transition;
        r = t->brightness;
    } else {
        r = 0;
    }
    return r;
}
