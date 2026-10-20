#include "global.h"

s32 Vorn_UpdateBehavior(void *);
u32 Vorn_UpdateActionTimers(u8 *);
void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);

struct S {
    u8 filler1[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Vorn_OnUpdate(struct S *s)
{
    u32 m;
    Vorn_UpdateBehavior(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        {
            u8 *c = (u8 *)s + 0xDC;
            u32 mask = ~0x20;
            c[0xF] &= mask;
        }
        SceneActorControl_DispatchControl(s);
    }
    Vorn_UpdateActionTimers((u8 *)s);
    return 1;
}
