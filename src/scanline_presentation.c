#include "scanline_presentation.h"

extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
#ifdef NONMATCHING
// fuzzy: 66.9%
// scope shape 38 (differences once low registers are set aside)
void ScanlinePresentation_UpdateNoise(struct ScanlinePresentation *p)
{
    s32 i;
    s16 *amplitude = &p->amplitude, *delta = &p->delta;
    for (i = 0; i < p->start; i++)
        p->rows[p->active][i] = 0;
    i = p->start;
    for (; i < p->end; i++) {
        u32 offset;
        s32 v;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        offset = gUnk_03005308 * 2;
        v = (*(u16 *)(offset + (u32)gUnk_0203B400) * p->amplitude) >> 18;
        p->rows[p->active][i] = v | (v << 8);
    }
    for (i = p->end; i < 160; i++)
        p->rows[p->active][i] = 0;
    *amplitude += *delta;
    if (*amplitude < 0)
        *amplitude = 0;
    else if (*amplitude > 128)
        *amplitude = 128;
}
#else
INCLUDE_ASM("asm/nonmatching", ScanlinePresentation_UpdateNoise);
#endif

s32 Div(s32, s32);

void ScanlinePresentation_UpdateFadeIn(struct ScanlinePresentation *s)
{
    if (s->mode == 0)
        gUnk_03004DA0 = Div(s->phase << 6, s->duration);
    s->phase++;
    if (s->phase > s->duration) {
        s->phase = s->duration;
        s->fade = 0;
    }
}

void ScanlinePresentation_UpdateFadeOut(struct ScanlinePresentation *s)
{
    u32 k;
    if (s->mode == 0)
        gUnk_03004DA0 = Div(s->phase << 6, s->duration);
    s->phase--;
    k = 0;
    if (s->phase < 0) {
        s->phase = k;
        s->fade = (void *)k;
        if (s->script != 0)
            Script_Call(s->script, 0);
        Task_Kill(s);
    }
}

void ScanlineEffect_SetSource(int, int, void *);

static inline void hide(struct ScanlineLayer *p)
{
    p->flags |= 1;
}

int ScanlinePresentation_Update(struct ScanlinePresentation *p)
{
    if (p->mode == 0)
        gUnk_030051D4 = gUnk_03004DA0;
    if (p->cancellable && (gUnk_03005260[1] & 3)) {
        hide(&p->layers[0]);
        hide(&p->layers[1]);
        hide(&p->layers[2]);
        hide(&p->layers[3]);
        hide(&p->layers[4]);
        hide(&p->layers[5]);
        if (p->mode == 0)
            gUnk_030051D4 = gUnk_03004DA0 = 0;
        if (p->script)
            Script_Call(p->script, 0);
        Task_Kill(p);
    } else {
        if (p->fade)
            p->fade(p);
        if (p->noise) {
            p->noise(p);
            ScanlineEffect_SetSource(0, 2, p->rows[p->active]);
            p->active = 1 - p->active;
        }
        switch (p->state) {
            case 0:
                p->timer++;
                if (p->timer > p->delay) {
                    p->state = 1;
                    p->timer = 0;
                    p->fade = ScanlinePresentation_UpdateFadeIn;
                    if (p->start != p->end)
                        Sound_PlaySong(225);
                    p->amplitude = 128;
                    p->delta = -2;
                    p->noise = ScanlinePresentation_UpdateNoise;
                }
                break;
            case 1:
                if (!p->fade)
                    p->state = 2;
                break;
            case 2:
                p->timer++;
                if (p->timer > p->hold) {
                    p->state = 3;
                    p->timer = 0;
                    p->fade = ScanlinePresentation_UpdateFadeOut;
                    if (p->start != p->end)
                        Sound_PlaySong(227);
                    p->amplitude = 0;
                    p->delta = 4;
                    p->noise = ScanlinePresentation_UpdateNoise;
                }
                break;
        }
        p->ticks++;
    }
    return 0;
}

void sub_082194D4(void *);

void Mosaic_ResetParameters(void);

int ScanlinePresentation_Destroy(struct ScanlinePresentation *p)
{
    int i;
    struct ScanlineLayer *layer;
    sub_082194D4(p);
    Mosaic_ResetParameters();
    Anim_Remove(&p->layers[0]);
    Anim_Remove(&p->layers[1]);
    Anim_Remove(&p->layers[2]);
    Anim_Remove(&p->layers[3]);
    layer = &p->layers[4];
    for (i = 1; i >= 0; i--) {
        Anim_Remove(layer);
        layer++;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatching", sub_0800DC90);

void *ScanlinePresentation_Create(void)
{
    struct ScanlinePresentation *p = Task_Create(8, 0x52C);
    if (p) {
        Task_SetCallbacks(p, (void *)ScanlinePresentation_Update,
                          (void *)ScanlinePresentation_Destroy);
        if (sub_0800DC90(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
