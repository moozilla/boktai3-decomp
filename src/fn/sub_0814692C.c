#include "global.h"
#include "player.h"
#include "ram.h"
// confidence=low: energy-gated action creates ActorBurst and sets the shared burst latch.
u32 Player_ConsumeStateEntry(struct Player *);
s32 Player_HasRequiredEnergy(struct Player *, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_UpdateFacingSide(struct Player *);
void ActorBurst_Create(struct Player *, struct SceneVec *, struct SceneVec *, s32, u32);
void Player_UpdateEnergyBurst(struct Player *p)
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
                Sound_PlaySong(0x2E4);
                p->actionEighteenParameter = 0x16F;
                p->actionEighteenDuration = 2;
                ActorBurst_Create(p, &p->position, &p->actionOrigin, 1, *(u16 *)((u8 *)p + 0x4A2));
            }
            ActorSequence_Begin(p, 9, 1);
            break;
        case 1:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x16F;
                if ((p->actionEighteenDuration += 3) > 32)
                    p->actionEighteenDuration = 32;
            }
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                Sound_PlaySong(0xCB);
                if (p->actionEighteenEnabled && !*(u16 *)((u8 *)p + 0x4A2))
                    gPlayerSolarActionPending = 1;
                ActorSequence_Begin(p, 9, 2);
            }
            break;
        case 2:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x16F;
                if ((p->actionEighteenDuration += 3) > 32)
                    p->actionEighteenDuration = 32;
            } else if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
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
