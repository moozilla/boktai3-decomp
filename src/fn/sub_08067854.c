#include "scene_actor_control.h"
extern void (*gUnk_08605B44[])(struct SceneActorControlManager *, struct SceneActorControlSlot *);
void SceneActorControl_ClearUnusedParameters(struct SceneActorControlManager *);
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
        SceneActorControl_ClearUnusedParameters(p);
    }
    return 0;
}
