#include "global.h"
struct S {
    u8 f[0x77c];
    u32 a;
};
void sub_081C2C04(void *);
void SparkleEmitter_OnUpdateHook(void *);
void Callback_InvokeActor(void *, u32);
void SparkleEmitter_OnTaskUpdate(struct S *p)
{
    sub_081C2C04(p);
    SparkleEmitter_OnUpdateHook(p);
    if (p->a != 0)
        Callback_InvokeActor(p, p->a);
}
