// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
// confidence=low: waits for a timer, then replaces response slot 0 and restarts the behavior.
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
#define Mummy_WaitReplaceResponse sub_080D5370
void Mummy_WaitReplaceResponse(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)actor + 0x3D4;
    asm("" : "+r"(group));
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    if ((s32)actor->counter2AC >= *(u16 *)(group + 0x44)) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, *((u8 *)actor + 0x98));
        Chase_SetBehavior(actor, 2);
    } else {
        actor->counter2AC++;
    }
}
