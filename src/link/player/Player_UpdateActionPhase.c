#include "player/player_status_flags.h"
#include "global.h"
#include "scene/areas/area_effect_link.h"

extern void *gUnk_08E87B0C[];
void Player_AccumulateActionCharge(struct Player *, s32);
void sub_082420D4(struct Player *);
s32 sub_08242D30(struct Player *);
void sub_0823B728(struct Player *, s32);
void Callback_InvokeActor(struct Player *, void *);
void Player_SynchronizeColliderState(struct Player *);
void Player_SmoothCameraAnchor(struct Player *);
void LinkPlayer_ClampToRecoveryTerrainCell(struct Player *);
void Player_UpdateModifier08Drift(struct Player *);
void Player_SetSecondaryActionFlags(struct Player *, s32);

static inline u8 Charge(s32 slot)
{
    if (gLinkSession == NULL || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}

void Player_UpdateActionPhase(struct Player *p)
{
    Player_AddStatusFlags(p, 0x54000);
    switch (p->kind) {
        case 1: {
            s32 r;
            if (p->statusFlags & 0x10) {
                Player_AccumulateActionCharge(p, Charge(p->slot));
            }
            sub_082420D4(p);
            r = sub_08242D30(p);
            sub_0823B728(p, r);
            Callback_InvokeActor(p, gUnk_08E87B0C[r]);
            if (p->movementVelocityLocked != 0) {
                *(struct SceneVec *)&p->velocityX = p->lockedVelocity;
            }
            Player_SynchronizeColliderState(p);
            if (p->cameraOffsetActive != 0)
                Player_SmoothCameraAnchor(p);
            LinkPlayer_ClampToRecoveryTerrainCell(p);
            Player_UpdateModifier08Drift(p);
            break;
        }
        case 2:
            Player_SetSecondaryActionFlags(p, 8);
            Callback_InvokeActor(p, (void *)p->triggerAnimationCallback);
            Player_SynchronizeColliderState(p);
            break;
    }
}
