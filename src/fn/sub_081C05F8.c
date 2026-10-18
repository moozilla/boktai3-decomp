#include "global.h"
struct S {
    u8 f[0x30];
    u32 a;
};
void Callback_InvokeActor(void *, u32);
void ScenePaletteFade_Update(struct S *p)
{
    Callback_InvokeActor(p, p->a);
}
