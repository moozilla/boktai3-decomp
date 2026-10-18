#include "global.h"
struct S {
    u8 filler[0x294];
    u32 *tbl;
    u8 f2[0x12];
    u8 idx;
};
void Callback_InvokeActor(void *, u32);
void SceneActorState_DispatchTable294State(struct S *p)
{
    u32 i = p->idx;
    Callback_InvokeActor(p, p->tbl[i]);
}
