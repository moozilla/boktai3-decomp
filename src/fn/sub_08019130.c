#include "global.h"
#include "ram.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Circle {
    u8 pad[24];
    s16 radius;
    u16 direction, step;
    u8 dispose, pad1f;
    u16 time, bank;
    u16 rows[2][160];
    struct Vec center;
};
extern u8 *gUnk_02000580[];
extern struct Vec gUnk_03005418;
int Script_GetKeywordValue(int, int);
void CircleWindow_GenerateScanlines(void *, int);
static inline int project(int input)
{
    int v = input * 3;
    if (v >= 0)
        v = v >> 5;
    else
        v = -((-v) >> 5);
    return v;
}
int CircleWindow_Initialize(struct Circle *p)
{
    int i;
    gCircleWindow = p;
    p->time = 0;
    p->bank = 0;
    p->direction = Script_GetKeywordValue('m', 0);
    p->radius = Script_GetKeywordValue('r', 250) * 16;
    if (p->radius >= 4000)
        p->radius = 4000;
    p->step = Script_GetKeywordValue('s', 8) * 16;
    p->dispose = Script_GetKeywordValue('d', 0);
    if (gUnk_02000580[0]) {
        struct Vec *out = &p->center;
        struct Vec *in = (void *)(gUnk_02000580[0] + 48);
        int xy;
        int height, x = in->x, z = in->z;
        out->x = project(x - z);
        xy = project(x + z);
        height = project(in->y);
        {
            int y = xy - height, z = xy + height;
            out->x = out->x - gUnk_03005418.x + 120;
            out->y = y - gUnk_03005418.y + 80;
            out->z = z - gUnk_03005418.z;
        }
        if (p->center.x < 0)
            p->center.x = 0;
        else if (p->center.x > 240)
            p->center.x = 240;
        if (p->center.y < 0)
            p->center.y = 0;
        else if (p->center.y > 160)
            p->center.y = 160;
    } else {
        p->center.x = 120;
        p->center.y = 80;
    }
    for (i = 0; i <= 1; i++)
        CircleWindow_GenerateScanlines(p, i);
}
