#include "scene_actor_reset.h"
#include "engine.h"
struct SlimeReactionContext {
    u8 unknown0[0x3d4];
    u16 timer, unknown3D6, unknown3D8, healthThreshold;
    u8 unknown3DC[0x28];
    u16 splitScript;
    u8 unknown406[4];
    u16 flags;
};
void Slime_EmitTargetedReactionBurst(void *), Slime_RunPendingActionScript(void *),
    SceneActorControl_OnIdle(void);
static inline void Clear(u16 *p, u32 mask)
{
    *p = mask & *p;
}
struct Bits {
    s16 bits : 16;
};
static inline void ClearReaction(u16 *p, u32 mask)
{
    ((struct Bits *)p)->bits &= mask;
}
static inline u8 Flag(struct SlimeReactionContext *s, u32 mask)
{
    if (s->flags & mask)
        return 1;
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 behavior = b;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 CollisionFlags(struct ContactCollider *p)
{
    return p->statRewardFlags;
}
static inline u32 MaskedDamage(struct SceneActorRegistryResetView *actor, u32 flags, u32 mask)
{
    return actor->field2DC & flags & mask;
}
static inline u32 Mask16(u16 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 DamageFlags(struct SceneActorRegistryResetView *actor, u32 offset)
{
    return *(u32 *)((u8 *)actor + offset);
}
// confidence=low: reaction callback chooses split/idle/recovery states after damage.
u32 Slime_ResolveDamageReaction(struct SceneActorRegistryResetView *actor)
{
    struct SlimeReactionContext *context = actor->context;
    u16 *motionFlags;
    u32 zero;
    u32 damage;
    u32 damageFlagsOffset;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x3bf);
    if (actor->paletteOffset == 3)
        Slime_EmitTargetedReactionBurst(actor);
    Clear(&actor->flags2D4, ~0x20);
    context->timer += 80;
    damage = MaskedDamage(actor, CollisionFlags(&actor->colliderA), 0x7f);
    motionFlags = &actor->flags2D4;
    damageFlagsOffset = (u32) & ((struct SceneActorRegistryResetView *)0)->field2DC;
    if (damage == 0 && context->splitScript) {
        if (actor->health < context->healthThreshold)
            Slime_RunPendingActionScript(actor);
        else {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                s32 random = *(const u8 *)(RANDOM_TABLE_ADDR + index * 2);
                if (random <= 24)
                    Slime_RunPendingActionScript(actor);
            }
        }
    }
    if ((DamageFlags(actor, damageFlagsOffset) & 0x40000)) {
        ClearReaction(&actor->flags2D6, ~0x980);
        SetControl(actor, actor->reactionTarget230, 0x12);
        SetState(actor, 3, 0, 0);
    } else {
        if (Mask16(motionFlags, 0x100))
            return 0;
        if (actor->health < context->healthThreshold || (zero = Flag(context, 8))) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 0, 2);
            ClearReaction(&actor->flags2D6, ~0x980);
        } else if ((*(u32 *)&actor->state2A8 & 0xffffff) != 0) {
            SetControl(actor, actor->savedTarget, 0);
            SetState(actor, 0, 0, 0);
        }
    }
    return 1;
}
