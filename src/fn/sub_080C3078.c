#include "global.h"

void SceneActorReaction_TickHiddenResponse(void *);
void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);
void Skeleton_UpdateActionFlags(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Skeleton_Update(struct S *s)
{
    u32 m;
    SceneActorReaction_TickHiddenResponse(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Skeleton_UpdateActionFlags(s);
    return 1;
}
