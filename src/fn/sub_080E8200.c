
#ifdef NONMATCHING
// fuzzy: 98.7%
// foundry draft (worker best 98.7%); what differs is mostly register allocation and instruction order
#define SceneActor_AcquireCollisionTarget sub_080E8200
#include "global.h"
#include "scene_actor_reaction.h"
#include "player.h"
extern u32 gUnk_030053F4;
void *SceneActorContacts_FindObjectById(u16);
u8 sub_080E7F54(struct SceneActorRegistryResetView *);
u8 sub_080E8444(struct SceneActorRegistryResetView *);
static inline u8 motionFlag(struct SceneActorRegistryResetView *actor)
{
    u8 *motion = (u8 *)&actor->fieldDC;
    u32 mask = 2;
    if (motion[15] & mask)
        return 1;
    return 0;
}
static inline u8 nearFlag(struct SceneActorRegistryResetView *actor)
{
    u8 *motion = (u8 *)&actor->fieldDC;
    u32 mask = 8;
    if (motion[15] & mask)
        return 1;
    return 0;
}
static inline u8 collisionMode(void)
{
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}
static inline void setTargetFlags(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x80;
    u16 *flags = &actor->flags2D6;
    u8 *motion;
    mask |= *flags;
    *flags = mask;
    motion = (u8 *)&actor->fieldDC;
    {
        u32 mask = 0x10;
        mask |= motion[15];
        motion[15] = mask;
    }
}
s32 sub_080E8200(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u8 *motion;
    void *target;
    s32 d;
    if (!motionFlag(actor))
        goto zero;
    if ((s32)actor->field304 > 0) {
        setTargetFlags(actor);
        actor->fieldDC = (u32)SceneActorContacts_FindObjectById((u16)actor->field304);
        return 1;
    }
    if (collisionMode()) {
        if (sub_080E7F54(actor))
            goto yes;
        if (!sub_080E8444(actor))
            goto collision;
        goto zero;
    } else {
        d = actor->position.y - gUnk_02000580[0]->position.y;
        if (d < 0)
            d = -d;
        if (d > 0x200)
            goto zero;
        if (!sub_080E7F54(actor))
            goto collision;
    }
yes:
    setTargetFlags(actor);
    return 1;
collision:
    motion = (u8 *)&actor->fieldDC;
    target = (void *)((struct SceneActorReactionMotion *)motion)->fieldDC;
    if (!target)
        goto zero;
    if (actor->distanceF0 >= actor->distanceLimitF8)
        goto zero;
    if (!sub_0821F204(*(void **)(group + 0x1FC), &actor->position,
                      (struct ContactPosition *)((u8 *)target + 12)))
        goto zero;
    if (!nearFlag(actor))
        goto zero;
    setTargetFlags(actor);
zero:
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E8200);
#endif
