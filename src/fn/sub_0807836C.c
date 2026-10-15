// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorBehavior_TickDrawableBlink sub_0807836C
extern const s16 gUnk_08605C20[3];
static inline u8 TestNonzero(u32 value)
{
    if (value)
        return 1;
    return 0;
}
u8 SceneActorBehavior_TickDrawableBlink(struct SceneActorRegistryActor *inputActor, s32 inputAge)
{
    struct SceneActorRegistryActor *actor = inputActor;
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
            kind = actor->drawableKind;
            if (kind == 0) {
                void *base = actor->drawable;
                u32 flags = ((struct SceneActorRegistrySequenceDrawable *)base)->flags;
                flags &= mask;
                drawable = base;
                hidden = TestNonzero(flags);
            } else {
                void *base = actor->drawable;
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
                u8 *base = (u8 *)drawable + 0x20;
                {
                    u32 mask = ~1;
                    *(u32 *)(base + 8) &= mask;
                }
            }
        } else {
            u32 mask = 1;
            asm("" : "+r"(mask));
            if (!kind)
                ((struct SceneActorRegistrySequenceDrawable *)drawable)->flags |= mask;
            else {
                register u8 *base asm("r1") = (u8 *)drawable + 0x20;
                asm("" : "+r"(base));
                *(u32 *)(base + 8) |= mask;
            }
        }
    }
    if (age > 60) {
        if (!actor->drawableKind) {
            void *base = actor->drawable;
            u32 mask = ~1;
            ((struct SceneActorRegistrySequenceDrawable *)base)->flags &= mask;
        } else {
            register u8 *base asm("r0") = (u8 *)actor->drawable + 0x20;
            u32 mask = ~1;
            *(u32 *)(base + 8) &= mask;
        }
        return 1;
    }
    return 0;
}
