#include "global.h"

void Dog_PrepareFrame(void *);
void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);
void Dog_FinishFrame(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
};

s32 Dog_Update(struct S *s)
{
    struct S *s2 = s;
    Dog_PrepareFrame(s);
    if (s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Dog_FinishFrame(s2);
    return 1;
}
