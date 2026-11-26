#include "engine/shared/global.h"
#include "engine/shared/player.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"
// confidence=low: varying height offset suggests hovering during interaction.
void Player_ClampInteractionVelocity(struct Player *);
void Obj_StepPosition(void *);
void Actor_EmitTowardNearbyActors(void *);
u32 Player_GetControlFlags(struct Player *, u16);
void Collider_SetPosition(void *, void *, u32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
s32 Mod(s32, s32);
static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline s32 hoverTarget(void)
{
    return Mod(randomValue() >> 3, 100) - 116;
}
void Player_UpdateInteractionHoverDisplay(struct Player *p)
{
    Player_ClampInteractionVelocity(p);
    Obj_StepPosition(&p->positionalEffectGroup);
    Actor_EmitTowardNearbyActors(&p->positionalEffectGroup);
    if (p->tetherMode <= 1) {
        u8 *timer = &p->colliderHeightRetargetTimer;
        if (!*timer) {
            p->colliderHeightTarget = hoverTarget();
            *timer = 16;
        } else
            --*timer;
        {
            u8 *height = &p->colliderHeightOffset;
            u32 current = *height;
            u32 target = p->colliderHeightTarget;
            if ((s32)current <= (s32)target - 4)
                *height = current + 4;
            else if ((s32)current >= (s32)target + 8)
                *height = current - 8;
            else
                *height = target;
        }
    }
    p->colliderPosition.y =
        (u16)p->position.y + p->colliderHeightOffset + p->terrainRecoveryCounter;
    if (Player_GetControlFlags(p, 8))
        p->collider.flags |= 4;
    else {
        p->collider.flags &= ~4;
        Collider_SetPosition(&p->collider, &p->position, 0);
    }
    if (Player_GetControlFlags(p, 1))
        p->secondaryDrawFlags |= 1;
    else
        p->secondaryDrawFlags &= ~1;
    if (Player_GetControlFlags(p, 0x40))
        p->animation.flags &= ~1;
    else
        p->animation.flags |= 1;
    if (Player_GetControlFlags(p, 4))
        ActorShadow_Disable(p->shadow);
    else
        ActorShadow_Enable(p->shadow);
}
