#include "monsters/sword/sword_task.h"

extern void (*const gUnk_0860643C[])(struct SwordComponent *);

void Sword_UpdateComponents(struct SwordTask *s)
{
    s32 i;
    for (i = 0; i < s->count; i++) {
        struct SwordComponent *e = &s->components[i];
        Callback_InvokeActor(e, gUnk_0860643C[e->active]);
    }
}
