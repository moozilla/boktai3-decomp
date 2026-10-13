#include "global.h"
struct Circle {
    u8 pad[24];
    s16 radius;
    u16 direction, step;
    u8 dispose, pad1f;
    u16 time, bank;
    u16 rows[2][160];
    s16 x, y;
};
extern void *gUnk_03004278, *gUnk_03003A00;
extern u16 gUnk_03004270, gUnk_03004274;
void sub_08018F38(void *, int);
void sub_0821A0C0(void *);
#define CircleWindow_Update sub_08019044
int CircleWindow_Update(struct Circle *p)
{
    p->x = 120;
    p->y = 80;
    sub_08018F38(p, 1 - p->bank);
    gUnk_03004278 = p->rows[p->bank];
    gUnk_03003A00 = (void *)0x04000040;
    gUnk_03004270 = 1;
    gUnk_03004274 = 1;
    if (p->direction) {
        p->radius += p->step;
        if (p->radius >= 4000) {
            p->radius = 4000;
            if (p->dispose)
                sub_0821A0C0(p);
        }
    } else {
        p->radius -= p->step;
        if (p->radius <= 512) {
            p->radius = 512;
            if (p->dispose)
                sub_0821A0C0(p);
        }
    }
    p->bank = 1 - p->bank;
    p->time++;
    return 0;
}
