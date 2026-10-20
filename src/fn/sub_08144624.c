#include "global.h"
#include "player.h"
struct BurstPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u32 Player_ConsumeStateEntry(struct Player *);
s32 Player_CanSpendSharedResource(struct Player *, s32);
void *SpiralBurst_Create(struct Player *, void *);
void Sound_PlaySong(u32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void ActorSequence_Begin(struct Player *, s32, s32);
extern const s16 gUnk_086149C4[];
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
// confidence=low: creates a spiral burst ahead of the player when two shared resource units are available.
void Player_UpdateSpiralBurst(struct Player *p)
{
    if (p->tetherMode != 0x11) {
        if (Player_CanSpendSharedResource(p, 2)) {
            struct BurstPosition pos;
            s32 angle;
            pos = *(struct BurstPosition *)&p->position;
            angle = ((p->facing + 5) & 7) << 5;
            pos.x += scale(gUnk_086149C4[(angle + 64) & 255] * 150);
            pos.z += scale(gUnk_086149C4[angle] * 150);
            SpiralBurst_Create(p, &pos);
            p->actionEighteenEnabled = 1;
        } else
            p->actionEighteenEnabled = 0;
        Sound_PlaySong(0x306);
        ActorSequence_Begin(p, 0x11, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x173;
                p->actionEighteenDuration = 32;
            }
            if (Player_SetAnimation(p, 0, 64, 1)) {
                if (p->actionEighteenEnabled)
                    ActorSequence_Begin(p, 0x11, 1);
                else
                    ActorSequence_Begin(p, 0x11, 2);
            }
            break;
        case 1:
            p->actionEighteenParameter = 0x173;
            p->actionEighteenDuration = 32;
            Player_SetAnimation(p, 0, 64, 1);
            if (++p->tetherFrame > 31)
                ActorSequence_Begin(p, 0x11, 3);
            break;
        case 2:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, 0, 64, 1);
            if (++p->tetherFrame > 15)
                ActorSequence_Begin(p, 0x11, 3);
            break;
        case 3:
            if (Player_SetAnimation(p, 0, 64, 1))
                ActorSequence_Begin(p, 0, 0);
            break;
    }
}
