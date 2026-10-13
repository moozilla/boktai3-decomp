#include "global.h"
typedef struct {
    u32 pad[2];
    u32 flags;
    u8 rest[84];
} Layer;
typedef struct Controller Controller;
struct Controller {
    u8 pad[0x38];
    u16 state, timer;
    u32 ticks, script;
    s16 cancellable, mode, phase, delay, hold, duration;
    u8 pad50[4];
    u16 rows[2][160];
    u16 active;
    s16 amplitude, delta;
    u16 start, end;
    u8 pad2de[6];
    void (*fade)(Controller *);
    void (*noise)(Controller *);
    Layer first, second, third, fourth, tail[2];
};
extern u32 gUnk_030051D4, gUnk_03004DA0;
extern u16 gUnk_03005260[];
void sub_0821AD08(u32, void *);
void sub_0821A0C0(void *);
void sub_08219254(int, int, void *);
void sub_0822B2F8(int);
void sub_0800D9A0(Controller *);
void sub_0800D9FC(Controller *);
void sub_0800D874(Controller *);
static inline void hide(Layer *p)
{
    p->flags |= 1;
}
int ScanlinePresentation_Update(Controller *p)
{
    if (p->mode == 0)
        gUnk_030051D4 = gUnk_03004DA0;
    if (p->cancellable && (gUnk_03005260[1] & 3)) {
        hide(&p->first);
        hide(&p->second);
        hide(&p->third);
        hide(&p->fourth);
        hide(&p->tail[0]);
        hide(&p->tail[1]);
        if (p->mode == 0)
            gUnk_030051D4 = gUnk_03004DA0 = 0;
        if (p->script)
            sub_0821AD08(p->script, 0);
        sub_0821A0C0(p);
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
                        sub_0822B2F8(225);
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
                        sub_0822B2F8(227);
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
