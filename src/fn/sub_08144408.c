#include "global.h"
#include "player.h"
#include "game_state.h"
struct BurstPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u32 Player_ConsumeStateEntry(struct Player *);
s32 Player_CanSpendSharedResource(struct Player *, s32);
void *ChargedBurst_Create(void *, u32, u32, u32, u32);
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
// confidence=low: creates a charged burst ahead of the player using the shared resource level.
void Player_UpdateChargedBurst(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (Player_CanSpendSharedResource(p, 0)) {
            struct BurstPosition pos;
            s32 angle, power;
            pos = *(struct BurstPosition *)&p->position;
            angle = ((p->facing + 5) & 7) << 5;
            pos.x += scale(gUnk_086149C4[(angle + 64) & 255] * 150);
            pos.z += scale(gUnk_086149C4[angle] * 150);
            power = gGameState->unk876 * 60 + 120;
            ChargedBurst_Create(&pos, 1 << p->movementRequestBlocked, power, angle, 90);
            p->actionEighteenEnabled = 1;
        } else
            p->actionEighteenEnabled = 0;
        Sound_PlaySong(0x306);
        Player_SetActionPhase(p, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x171;
                p->actionEighteenDuration = 32;
            }
            if (Player_SetAnimation(p, 0, 64, 1)) {
                if (p->actionEighteenEnabled)
                    Player_SetActionPhase(p, 1);
                else
                    Player_SetActionPhase(p, 2);
            }
            break;
        case 1:
            p->actionEighteenParameter = 0x171;
            p->actionEighteenDuration = 32;
            Player_SetAnimation(p, 0, 64, 1);
            if (++p->tetherFrame > 31)
                Player_SetActionPhase(p, 3);
            break;
        case 2:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, 0, 64, 1);
            if (++p->tetherFrame > 15)
                Player_SetActionPhase(p, 3);
            break;
        case 3:
            if (Player_SetAnimation(p, 0, 64, 1))
                ActorSequence_Begin(p, 0, 0);
            break;
    }
}
