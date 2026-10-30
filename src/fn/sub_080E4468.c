// confidence=low: tracks player zero and switches from the chase animation to a bite in range.
#include "global.h"
/* Dog_ChasePlayer: on entry starts the chase animation (kind 0x28); bites when a bite frame of the run cycle ends with player 0 in range (records the player's position, extends the chase timer), otherwise once the control finishes either follows the group's chase (flags2D4 & 0x80) or routes back to its route point (provisional name) */
#include "scene_actor_chase.h"

void Dog_UpdateAnimationFourteenReaction(void);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

struct DogAnim {
    u8 unknown0[7];
    u8 rate;
    u16 frame;
    u8 unknownA[4];
    u16 timer;
};

static inline u16 LoadMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 value = actor->flags2D4;
    return value & mask;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

void Dog_ChasePlayer(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u8 *group = (u8 *)&actor->group;
    struct ContactPosition target;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, Dog_UpdateAnimationFourteenReaction, 0x28);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    {
        struct DogAnim *anim;
        s32 distance;
        Delta(&target, &actor->body.position, (struct ContactPosition *)((u8 *)gPlayers[0] + 0x30));
        distance = target.x * target.x + target.z * target.z;
        anim = (struct DogAnim *)((u8 *)actor->drawable + 0x48);
        if ((anim->frame == 6 || anim->frame == 0xC || anim->frame == 0x13 ||
             anim->frame == 0x19) &&
            anim->timer == anim->rate - 1 && (u32)distance < *(u32 *)(context + 0x684)) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
            Chase_SetBehavior(actor, 0x10);
            *(struct ContactPosition *)(context + 0x6D8) =
                *(struct ContactPosition *)((u8 *)gPlayers[0] + 0x30);
            {
                u32 chasing = 0x80;
                if (actor->flags2D4 & chasing) {
                    u16 *time = (u16 *)((u8 *)actor + 0x418);
                    *time += 0x12C;
                    if (*time > *(u16 *)((u8 *)actor + 0x416))
                        *time = *(u16 *)((u8 *)actor + 0x416);
                }
            }
            return;
        }
    }
    if (actor->field2B0) {
        if (LoadMotionFlags(actor, 0x80)) {
            Chase_SetBehavior(actor, 0xC);
            Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
            {
                u16 *field = (u16 *)(group + 0x40);
                u32 mask = 4;
                mask |= *field;
                *field = mask;
            }
            return;
        }
        Chase_SnapToRoute(&target, group + 0x74);
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 2);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, &target))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
    } else
        actor->counter2AC++;
}
