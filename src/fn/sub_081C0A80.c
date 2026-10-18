#include "global.h"
struct S {
    u8 f[0x50];
    u32 cb;
};
void SceneActorSequence_UpdateBackgroundScroll(void *);
void sub_0824923C(void *, u32);
void SceneActorSequence_Update(struct S *p)
{
    SceneActorSequence_UpdateBackgroundScroll(p);
    if (p->cb != 0)
        sub_0824923C(p, p->cb);
}
