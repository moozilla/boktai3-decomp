#include "global.h"
#ifdef NONMATCHING
// fuzzy: 75.2%
// Golem rest state: restores after a delay, then either chases (Chase_Arrive from
// include/scene_actor_chase.h; Arrive's result sets field2C2) or snaps back to its route
// point with Chase_TryRoute. The +0x2B0 == 0 increment is the shared tail at 080CEAA8.
#include "scene_actor_chase.h"

u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void ProximityVolume_TranslateBounds(void *, struct ContactPosition *);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void sub_0807C72C(struct SceneActorRegistryResetView *, u32, u32, u32);

void sub_080CE188(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct ContactPosition *position;

    if (actor->field2B1)
        actor->field2B1 = 0;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0 == 0) {
        actor->counter2AC++;
        return;
    }
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    position = &actor->position;
    ProximityVolume_TranslateBounds(*(void **)(group + 0x1FC), position);
    if (Chase_MotionFlags(actor, 0x80)) {
        {
            u8 *saved = group + 0x8C;
            *(u32 *)(saved + 0xC) = 0x1E;
            *(void **)(saved + 0x10) = *(void **)(group + 0x48);
            saved[8] = 0;
            saved[9] = 0;
            saved[10] = 0xC;
        }
        if (Chase_Test(gUnk_030053F4, 0x4800)) {
            if (!SceneActorTarget_TrySelectNearestParticipant(actor))
                return;
        } else
            actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
        if (Chase_Arrive(actor))
            actor->field2C2 = 0x20;
    } else {
        if (actor->flags2D4 & 0x200) {
            if (actor->flags2CC & 0x200000) {
                u8 *link = group + 0x4C;
                actor->flags2D0 &= ~0x1000000;
                actor->flags2D4 &= ~0x200;
                link[0xA] = 0;
                *(u32 *)(link + 0x18) = 0;
                *(u16 *)(link + 8) = 0;
            }
            sub_0807C72C(actor, 0, 2, actor->category);
        }
        actor->flags2CC |= 0x10;
        actor->field2C2 = 0x40;
        {
            u8 *saved = group + 0x8C;
            *(u32 *)(saved + 0xC) = 0x3C;
            *(void **)(saved + 0x10) = actor->entryTarget228;
            saved[8] = 0;
            saved[9] = 0;
            saved[10] = 7;
        }
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->pad = 1;
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), position,
                            *(struct ContactPosition **)(group + 0x200)))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080CE188);
#endif
