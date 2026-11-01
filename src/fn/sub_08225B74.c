#include "global.h"
#include "render.h"

struct Obj {
    u8 pad0[8];
    u8 f8;
    u8 pad9[3];
    u16 fC;
    u16 fE;
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    u16 f18;
    u8 pad1A[2];
    void *f1C;
    u16 f20;
    u16 f22;
    u8 pad24[8];
    u32 *f2C;
    u32 *f30;
};
void Terrain_MoveFootprint(void *, void *, void *, u16, u16, u8);

void Obj_StepPosition(struct Obj *o)
{
    void *p = o->f1C;
    if (p != 0) {
        switch (gCamera.mode) {
            case 0:
                Terrain_MoveFootprint(p, &o->fC, &o->f14, o->f20, o->f22, o->f8);
                break;
            case 1:
                o->fC += o->f14;
                o->fE += o->f18;
                o->f10 = 0;
                break;
        }
    } else {
        o->fC += o->f14;
        o->fE += o->f16;
        o->f10 += o->f18;
    }
    {
        u32 *q = o->f2C;
        if (q != 0) {
            u32 x = *(u32 *)&o->fC;
            u32 y = *(u32 *)&o->f10;
            q[7] = x;
            q[8] = y;
        }
    }
    {
        u32 *q = o->f30;
        if (q != 0) {
            u32 x = *(u32 *)&o->fC;
            u32 y = *(u32 *)&o->f10;
            q[8] = x;
            q[9] = y;
        }
    }
    o->f14 = 0;
    o->f16 = 0;
    o->f18 = 0;
}
