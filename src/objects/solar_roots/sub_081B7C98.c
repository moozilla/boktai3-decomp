#include "global.h"

typedef struct {
    u8 pad0[0x2c];
    u8 obj[6];
    u16 f32;
    u8 pad1[4];
    void *f38;
    u8 pad2[0xf6 - 0x3c];
    u16 idx;
    u8 pad3[8];
    u8 cnt;
    u8 pad4[0x114 - 0x101];
    void *arg;
} Ent;

typedef struct {
    u8 pad0[0x18];
    u8 n;
    u8 pad1[0x5c - 0x19];
    Ent e[2];
    u8 pad2[0x4bc - 0x5c - 2 * 0x118];
    u8 *q;
} Obj;

extern void GrowthHazard_UpdateSharedFlash(Obj *p);
extern s32 Callback_InvokeActor(void *arg, void *function);
extern void AnimData_SelectPaletteBank(void *a, s32 b);
extern u32 gUnk_08612154[];

s32 GrowthHazard_UpdateManager(Obj *p)
{
    s32 i;
    GrowthHazard_UpdateSharedFlash(p);
    if (*(s16 *)(p->q + 0x94) > 0) {
        for (i = 0; i < p->n; i++) {
            Ent *e = &p->e[i];
            Callback_InvokeActor(e, e->arg);
            if (e->cnt != 0) {
                e->cnt--;
                if (e->cnt != 0) {
                    e->f32 = gUnk_08612154[e->idx];
                    e->f38 = (u8 *)p + (e->idx * 32 + 0x1c);
                } else {
                    AnimData_SelectPaletteBank(&e->obj, 0xd5);
                }
            }
        }
    }
    return 0;
}
