#include "global.h"
#include "player.h"
// confidence=low: restores the tentative health-like stat A using two stat B units.
u32 Player_ConsumeStateEntry(struct Player *);
void *SpiralBurst_Create(struct Player *, struct SceneVec *);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateStatARestoreAction(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if ((u8)(p->facing - 2) <= 4) {
            p->facing = 4;
            p->heading = 1;
            p->side = 0;
        } else {
            p->facing = 0;
            p->heading = 0;
            p->side = 0;
        }
        if (p->currentStatB > 1) {
            struct SceneVec *position = &p->position;
            SpiralBurst_Create(p, position);
            p->actionEighteenEnabled = 1;
            if (p->currentStatA < p->maximumStatA) {
                s32 old = p->currentStatA;
                s32 value = old + 40;
                if (value >= p->maximumStatA)
                    value = p->maximumStatA;
                DamageNumberDisplay_Show(&p->collider, value - old, 1, position,
                                         &p->movementRequestTarget, 0);
                p->currentStatA = value;
            }
            p->currentStatB -= 2;
        } else
            p->actionEighteenEnabled = 0;
        Sound_PlaySong(0x306);
    }
    switch (p->tetherPhase) {
        case 0:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x173;
                p->actionEighteenDuration = 32;
            }
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                if (p->actionEighteenEnabled)
                    Player_SetActionPhase(p, 1);
                else
                    Player_SetActionPhase(p, 2);
            }
            break;
        case 1:
            p->actionEighteenParameter = 0x173;
            p->actionEighteenDuration = 32;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            if (++p->tetherFrame > 23)
                Player_SetActionPhase(p, 3);
            break;
        case 2:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            if (++p->tetherFrame > 15)
                Player_SetActionPhase(p, 3);
            break;
        case 3:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
