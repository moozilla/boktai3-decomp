#include "global.h"

struct S {
    u8 f[0x70];
    u32 a;
};
void Trigger_ConsumePending(void *, u32);
void SceneMessages_SetSignal(struct S *p, void *q)
{
    p->a = 1;
    Trigger_ConsumePending(q, 1);
}
