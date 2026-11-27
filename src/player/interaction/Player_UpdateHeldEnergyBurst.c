#include "global.h"
#include "player/player.h"
#include "engine/ram.h"

// confidence=low: energy-gated burst sequence with a held middle animation.
s32 Player_ConsumeStateEntry(void *);
s32 Player_HasRequiredEnergy(void *, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_UpdateFacingSide(void *);
void Sound_PlaySong(u32);
void ActionFlashBurst_Create(void *, void *, void *, u32, u32);
static inline void ResetBurstSize(struct Player *p)
{
    p->actionEighteenParameter = 0x16E;
}
static inline void grow(struct Player *p)
{
    ResetBurstSize(p);
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
}
void Player_UpdateHeldEnergyBurst(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->actionVariant = 0;
        if (Player_HasRequiredEnergy(p, 100) && !gPlayerSolarActionPending)
            p->actionEighteenEnabled = 1;
        else
            p->actionEighteenEnabled = 0;
        ActorSequence_Begin(p, 9, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if ((u8)(p->facing - 2) <= 4) {
                p->facing = 4;
                p->heading = 4;
                p->side = 0;
                p->actionOrigin = p->position;
                p->actionOrigin.x -= 20;
                p->actionOrigin.y += 330;
                p->actionOrigin.z += 60;
            } else {
                p->facing = 0;
                p->heading = 0;
                p->side = 0;
                p->actionOrigin = p->position;
                p->actionOrigin.x += 20;
                p->actionOrigin.y += 330;
                p->actionOrigin.z -= 20;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            if (p->facing == 4)
                p->heading = 1;
            else
                p->heading = 0;
            if (p->actionEighteenEnabled) {
                Sound_PlaySong(0x128);
                ResetBurstSize(p);
                p->actionEighteenDuration = 2;
                ActionFlashBurst_Create(p, &p->position, &p->actionOrigin, 0, p->burstParameter);
            }
            ActorSequence_Begin(p, 9, 1);
            break;
        case 1:
            if (p->actionEighteenEnabled)
                grow(p);
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                Sound_PlaySong(0xCB);
                if (p->actionEighteenEnabled && !p->burstParameter)
                    gPlayerSolarActionPending = 1;
                ActorSequence_Begin(p, 9, 2);
            }
            break;
        case 2:
            if (p->actionEighteenEnabled)
                grow(p);
            else if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            p->resetValue6FC = 30;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            if (++p->tetherFrame > 23)
                ActorSequence_Begin(p, 9, 3);
            break;
        case 3:
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
