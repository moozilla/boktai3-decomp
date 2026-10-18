#include "global.h"

void SceneActorBehavior_CallActionScript(u8 *);

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

struct S {
    u8 pad0[0x59];
    u8 at59;
    u8 pad1[0xE9 - 0x5A];
    u8 atE9;
    u8 pad3[0x2AC - 0xEA];
    u32 counter;
    u8 done;
    u8 queued;
    u8 pad7[0x3D0 - 0x2B2];
    u8 *ctx;
};

void Bat_UpdateActionScript(struct S *obj)
{
    u8 *ctx = obj->ctx;

    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        obj->at59 = obj->atE9;
        SceneActorBehavior_CallActionScript((u8 *)obj);
    }
    if (*(u8 *)((u8 *)obj + 0x2B0)) {
        u32 flag = 4;
        u16 *h = (u16 *)(ctx + 0x40A);
        flag |= *h;
        *h = flag;
    } else {
        ++obj->counter;
    }
}
