#include "scene/actor/scene_actor_registry.h"
/* SceneActorBehavior_UpdateDrawableBlink: Toggles drawable visibility at age-dependent intervals and restores visibility after age 60 */
#define SceneActorBehavior_UpdateDrawableBlink sub_0807836C
#define SceneActorBehavior_TickDrawableBlink   sub_0807836C
extern const s16 gUnk_08605C20[3];
static inline u8 TestNonzero(u32 value)
{
    if (value)
        return 1;
    return 0;
}
u8 SceneActorBehavior_TickDrawableBlink(struct ChaseActor *inputActor, s32 inputAge)
{
    struct ChaseActor *actor = inputActor;
    s32 age = inputAge;
    s32 cadence = 0;
    if (age <= 30) {
        cadence = 2;
        if (age > 10)
            cadence = 1;
    }
    if (!Mod(age, gUnk_08605C20[cadence])) {
        void *drawable;
        u32 kind;
        u32 hidden;
        {
            u32 mask = 1;
            kind = actor->base.drawableKind;
            if (kind == 0) {
                void *base = actor->base.drawable;
                u32 flags = ((struct SceneActorRegistrySequenceDrawable *)base)->flags;
                flags &= mask;
                drawable = base;
                hidden = TestNonzero(flags);
            } else {
                void *base = actor->base.drawable;
                u32 flags = ((struct SceneActorRegistryTileDrawable *)base)->flags;
                flags &= mask;
                drawable = base;
                hidden = TestNonzero(flags);
            }
        }
        if (hidden) {
            if (!kind) {
                u32 mask = ~1;
                ((struct SceneActorRegistrySequenceDrawable *)drawable)->flags &= mask;
            } else {
                struct AnimSprite *tile = (void *)((u8 *)drawable + 0x20);
                {
                    u32 mask = ~1;
                    tile->flags &= mask;
                }
            }
        } else {
            u32 mask = 1;
            if (!kind)
                ((struct SceneActorRegistrySequenceDrawable *)drawable)->flags |= mask;
            else {
                struct AnimSprite *tile = (void *)((u8 *)drawable + 0x20);
                tile->flags |= mask;
            }
        }
    }
    if (age > 60) {
        if (!actor->base.drawableKind) {
            void *base = actor->base.drawable;
            u32 mask = ~1;
            ((struct SceneActorRegistrySequenceDrawable *)base)->flags &= mask;
        } else {
            struct AnimSprite *tile = (void *)((u8 *)actor->base.drawable + 0x20);
            u32 mask = ~1;
            tile->flags &= mask;
        }
        return 1;
    }
    return 0;
}
