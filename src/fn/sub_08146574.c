#include "global.h"
#include "player.h"
#define Player_UpdateInputCadenceCue sub_08146574
// confidence=low: shifts input history and adjusts animation rate from occupied samples.
u32 Player_ConsumeStateEntry(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Sound_PlaySong(u32);
void Player_AddControlFlags(struct Player *, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
struct CadenceHistory {
    u8 pad[0x500];
    u8 samples[32];
};
void Player_UpdateInputCadenceCue(struct Player *p)
{
    Player_ConsumeStateEntry(p);
    switch (p->tetherPhase) {
        case 0: {
            s32 i;
            p->facing = 7;
            p->heading = 1;
            p->side = 1;
            p->displaySegmentCount = 0;
            for (i = 31; i > 0; i--) {
                if (((struct CadenceHistory *)p)->samples[i - 1])
                    p->displaySegmentCount++;
                ((struct CadenceHistory *)p)->samples[i] =
                    ((struct CadenceHistory *)p)->samples[i - 1];
            }
            if (p->inputKeys[1] & p->interactionPromptFlags) {
                ((struct CadenceHistory *)p)->samples[0] = 1;
                p->displaySegmentCount++;
            } else
                ((struct CadenceHistory *)p)->samples[0] = 0;
            if (p->displaySegmentCount) {
                if (p->displaySegmentCount <= 3)
                    p->indicatorUpdateRate = 128 - (p->displaySegmentCount << 4);
                else if (p->displaySegmentCount <= 5)
                    p->indicatorUpdateRate = 64 - ((p->displaySegmentCount - 2) << 3);
                else if (p->displaySegmentCount <= 7)
                    p->indicatorUpdateRate = 32 - ((p->displaySegmentCount - 4) << 2);
                else
                    p->indicatorUpdateRate = 16;
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1),
                                    p->indicatorUpdateRate, 1);
                if (!p->actionCueCounter) {
                    if (!p->actionCueFlags)
                        Sound_PlaySong(0x28E);
                    else if (p->actionCueFlags == 3)
                        Sound_PlaySong(0x28D);
                }
            } else {
                p->indicatorUpdateRate = 0;
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0),
                                    p->indicatorUpdateRate, 1);
            }
            break;
        }
        case 1:
            if (!p->tetherFrame) {
                p->facing = 7;
                p->heading = 0;
                p->side = 1;
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 64, 1);
                Sound_PlaySong(0x1AD);
                p->terrainRecoveryCounter = 0;
            }
            Player_AddControlFlags(p, 4);
            if (++p->tetherFrame > 5) {
                Sound_PlaySong(0x1AE);
                if (p->actionMode == 6)
                    Sound_PlaySong(0x2EC);
                else
                    Sound_PlaySong(0x304);
                ActorSequence_Begin(p, 0x25, 2);
            }
            break;
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x12), 64, 1);
            p->terrainRecoveryCounter -= 60;
            Player_AddControlFlags(p, 4);
            if (++p->tetherFrame > 29) {
                Player_AddControlFlags(p, 1);
                p->terrainRecoveryCounter = 1500;
                ActorSequence_Begin(p, 0x25, 3);
            }
            break;
        case 3:
            Player_AddControlFlags(p, 5);
            p->tetherFrame++;
            break;
        case 4:
            p->facing = 7;
            p->heading = 1;
            p->side = 1;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            p->terrainRecoveryCounter = 0;
            p->tetherFrame++;
            break;
    }
}
