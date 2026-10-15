#include "scene_actor_control.h"
#define SceneActorControl_Update sub_08067854
extern void (*gUnk_08605B44[])(struct SceneActorControlManager *, struct SceneActorControlSlot *);
void sub_080671E4(struct SceneActorControlManager *);
s32 SceneActorControl_Update(struct SceneActorControlManager *p)
{
    struct SceneActorControlSlot *slots = p->slots;
    if (p->used) {
        s32 i;
        for (i = 0; i < 8; i++) {
            if (SceneActorControl_IsUsed(p, i)) {
                struct SceneActorControlSlot *s = (void *)(i * sizeof(*slots) + (u32)slots);
                sub_08249240(p, s, gUnk_08605B44[s->kind]);
            }
        }
        sub_080671E4(p);
    }
    return 0;
}
