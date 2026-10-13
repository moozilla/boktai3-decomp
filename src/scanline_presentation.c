#include "scanline_presentation.h"

INCLUDE_ASM("asm/nonmatching", sub_0800D874);

s32 Div(s32, s32);

void sub_0800D9A0(struct ScanlinePresentation *s)
{
    if (s->mode == 0)
        gUnk_03004DA0 = Div(s->phase << 6, s->duration);
    s->phase++;
    if (s->phase > s->duration) {
        s->phase = s->duration;
        s->fade = 0;
    }
}

void sub_0800D9FC(struct ScanlinePresentation *s)
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

void sub_08219254(int, int, void *);

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
            sub_08219254(0, 2, p->rows[p->active]);
            p->active = 1 - p->active;
        }
        switch (p->state) {
            case 0:
                p->timer++;
                if (p->timer > p->delay) {
                    p->state = 1;
                    p->timer = 0;
                    p->fade = sub_0800D9A0;
                    if (p->start != p->end)
                        Sound_PlaySong(225);
                    p->amplitude = 128;
                    p->delta = -2;
                    p->noise = sub_0800D874;
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
                    p->fade = sub_0800D9FC;
                    if (p->start != p->end)
                        Sound_PlaySong(227);
                    p->amplitude = 0;
                    p->delta = 4;
                    p->noise = sub_0800D874;
                }
                break;
        }
        p->ticks++;
    }
    return 0;
}

void sub_082194D4(void *);

void sub_0821910C(void);

int ScanlinePresentation_Destroy(struct ScanlinePresentation *p)
{
    int i;
    struct ScanlineLayer *layer;
    sub_082194D4(p);
    sub_0821910C();
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

void *sub_0800E08C(void)
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
