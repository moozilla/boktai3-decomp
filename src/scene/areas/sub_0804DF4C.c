#include "global.h"
#include "scene/areas/area_effect.h"
struct E {
    u8 f[0xc6];
    u8 on;
    u8 a[5];
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
};
struct O {
    u8 f[0x26];
    u8 n;
};
s32 AreaEffect_IsPositionCovered(struct SceneVec *p)
{
    struct O *o = gAreaEffectManager;
    s32 i;
    s32 n;
    if (o != 0)
        goto go;
    goto no;
yes:
    return 1;
go:
    i = 0;
    if (i >= o->n)
        goto no;
    n = o->n;
    do {
        struct E *e = (struct E *)((u8 *)o + i * 0xa8);
        if (e->on != 0) {
            s32 x = p->x;
            if (x >= e->x0) {
                s32 y = p->z;
                if (y >= e->y0) {
                    if (x <= e->x1) {
                        if (y <= e->y1)
                            goto yes;
                    }
                }
            }
        }
        i++;
    } while (i < n);
no:
    return 0;
}
