#include "global.h"
struct S {
    u8 filler[0x290];
    u32 *tbl;
    u8 f2[0x15];
    u8 idx;
};
void Callback_InvokeActor(void *, u32);
void SceneActorState_DispatchTable290State(struct S *p)
{
    u32 i = p->idx;
    Callback_InvokeActor(p, p->tbl[i]);
}
