// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
u16 Route_GetCurrentPointFacing(void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
static inline u32 Flags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}
static inline s32 Direction(s32 value, s32 mask)
{
    return ((value + 0x60) >> 6) & mask;
}
// confidence=low: aligns idle facing with the saved route direction after animation completion.
void Vorn_UpdateIdleFacing(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)actor + 0x50;
    void *route = (u8 *)actor + 0x448;
    u16 *scale;
    if (SceneActorReaction_TakePrepared(actor)) {
        s32 direction;
        s32 facing;
        actor->body.direction = (((actor->body.direction + 0x60) >> 6) & 3) * 64 - 64;
        direction = ((actor->body.direction + 0x60) >> 6) & 3;
        if (actor->behavior2AA != 20 && actor->behavior2AA != 6 && actor->behavior2AA != 16 &&
            !Flags(actor, 0x4080)) {
            facing = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            *((u8 *)actor + 0x3CD) = facing;
        } else
            facing = *((u8 *)actor + 0x3CD);
        if (direction == (facing >> 1)) {
            actor->field2B0 = 1;
            SceneActorAnimation_SelectDirectionalEntry(actor, 0);
            return;
        }
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
        actor->field2B3 = 0;
    }
    if (Flags(actor, 0xA02F)) {
        u16 *p = &actor->field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *p = &actor->field2C2;
        u16 value = *p;
        scale = p;
        if (!value)
            *scale = 64;
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    if (actor->field2B3) {
        group[9] = (*((u8 *)actor + 0x3CD) + 5) << 5;
        actor->field2B0 = 1;
        {
            register s32 facing asm("r1") = actor->body.direction + 0x60;
            asm("" : "+r"(facing));
            facing >>= 6;
            {
                s32 mask = 3;
                actor->body.direction = (mask & facing) * 64 - 64;
            }
        }
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    }
}
