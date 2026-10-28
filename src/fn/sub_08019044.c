#include "global.h"
#include "render.h"
struct Circle {
    u8 pad[24];
    s16 radius;
    u16 direction, step;
    u8 dispose, pad1f;
    u16 time, bank;
    u16 rows[2][160];
    s16 x, y;
};
void CircleWindow_GenerateScanlines(void *, int);
void Task_Kill(void *);
int CircleWindow_Update(struct Circle *p)
{
    p->x = 120;
    p->y = 80;
    CircleWindow_GenerateScanlines(p, 1 - p->bank);
    gScanlineDmaSource = p->rows[p->bank];
    gScanlineDmaTarget = (void *)0x04000040;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 1;
    if (p->direction) {
        p->radius += p->step;
        if (p->radius >= 4000) {
            p->radius = 4000;
            if (p->dispose)
                Task_Kill(p);
        }
    } else {
        p->radius -= p->step;
        if (p->radius <= 512) {
            p->radius = 512;
            if (p->dispose)
                Task_Kill(p);
        }
    }
    p->bank = 1 - p->bank;
    p->time++;
    return 0;
}
