#include "engine/shared/global.h"
#include "engine/shared/player.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"

s32 Player_GetControlFlags(void *, u32);
void CellObject_Init(void *, void *);
void Obj_StepPosition(void *);
void Actor_EmitTowardNearbyActors(void *);
void Collider_SetPosition(void *, void *, s32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void ActorDebris_Show(void *);
void ActorDebris_Hide(void *);
void PlayerEffect_SetPaletteSource(void *, u32, u32);

void Player_ApplyControlPresentation(struct Player *p)
{
    u32 value = p->tetherStatusFlags;
    if (value == 0) {
        if (Player_GetControlFlags(p, 16))
            p->followTarget = (void *)value;
        else if (!p->followTarget) {
            void *position = &p->movementContactFlags;
            CellObject_Init(position, &p->position);
            p->followTarget = position;
        }
    }
    Obj_StepPosition(&p->positionalEffectGroup);
    Actor_EmitTowardNearbyActors(&p->positionalEffectGroup);
    if (Player_GetControlFlags(p, 8)) {
        p->collider.flags |= 4;
    } else {
        p->collider.flags &= ~4;
        Collider_SetPosition(&p->collider, &p->position, 0);
    }
    if (Player_GetControlFlags(p, 1))
        SceneInputWidget_Hide(&p->animation);
    else
        SceneInputWidget_Show(&p->animation);
    if (Player_GetControlFlags(p, 2))
        p->animation.flags |= 0x200;
    else
        p->animation.flags &= ~0x200;
    if (Player_GetControlFlags(p, 64))
        p->secondaryDrawFlags &= ~1;
    else
        p->secondaryDrawFlags |= 1;
    if (Player_GetControlFlags(p, 4) || !p->resetState740)
        ActorShadow_Disable(p->shadow);
    else
        ActorShadow_Enable(p->shadow);
    if (Player_GetControlFlags(p, 256) && !Player_GetControlFlags(p, 1)) {
        ActorDebris_Show(p->actionDisplay);
        PlayerEffect_SetPaletteSource(p->actionDisplay, p->animation.paletteBank,
                                      (u32)p->animation.palette);
    } else
        ActorDebris_Hide(p->actionDisplay);
    p->position.y += p->actionCountdown;
    p->animation.position = *(struct ContactPosition *)&p->position;
    p->animation.position.x += p->formCueParameterA;
    p->animation.position.y += p->terrainRecoveryCounter;
    p->animation.position.z += p->formCueParameterB;
    if (p->contactCueDuration) {
        s32 jitter;
        u32 random;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        random = *(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
        jitter = (random & 15) - 7;
        p->animation.position.x += jitter;
        p->animation.position.z -= jitter;
        p->contactCueDuration--;
    }
}
