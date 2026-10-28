// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
// confidence=low: varying height offset suggests hovering during interaction.
void Player_ClampInteractionVelocity(struct Player *);
void Obj_StepPosition(void *);
void sub_0802E04C(void *);
u32 Player_GetControlFlags(struct Player *, u16);
void Collider_SetPosition(void *, void *, u32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
s32 Mod(s32, s32);
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
static inline s32 randomValue(void)
{
    u32 offset;
    register u32 base asm("r3");
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    offset = gRandomIndex << 1;
    base = (u32)gUnk_0203B400;
    return *(u16 *)(offset + base);
}
static inline s32 hoverTarget(void)
{
    return Mod(randomValue() >> 3, 100) - 116;
}
void Player_UpdateInteractionHoverDisplay(struct Player *p)
{
    Player_ClampInteractionVelocity(p);
    Obj_StepPosition(&p->positionalEffectGroup);
    sub_0802E04C(&p->positionalEffectGroup);
    if (p->tetherMode <= 1) {
        u8 *timer = (u8 *)p + 0x468;
        if (!*timer) {
            s32 value = hoverTarget();
            register u32 offset asm("r2") = 0x467;
            register u8 *target asm("r1");
            asm("" : "+r"(offset));
            target = (u8 *)p + offset;
            *target = value;
            *timer = 16;
        } else
            --*timer;
        {
            u8 *height = (u8 *)p + 0x466;
            u32 current = *height;
            u32 target = *((u8 *)p + 0x467);
            if ((s32)current <= (s32)target - 4)
                *height = current + 4;
            else if ((s32)current >= (s32)target + 8)
                *height = current - 8;
            else
                *height = target;
        }
    }
    p->colliderPosition.y = (u16)p->position.y + *((u8 *)p + 0x466) + p->terrainRecoveryCounter;
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
