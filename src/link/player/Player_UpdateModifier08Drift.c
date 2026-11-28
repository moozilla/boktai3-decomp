#include "global.h"
#include "player/player.h"

/* tentative: drifts the player along a direction while global modifier 08 is set */

extern const s16 gUnk_086149C4[];
u32 Player_IsFlaggedWithGlobalModifier08(u32 x);
void Player_PlayLocalSong(s32, u32);
s32 Actor_ProbeDiagonalTurnDir(struct Player *obj);

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void Player_UpdateModifier08Drift(struct Player *p)
{
    u32 angle;
    const s16 *t;

    if (p->movementVelocityLocked) {
        if (!Player_IsFlaggedWithGlobalModifier08(p->movementStatus)) {
            p->unk48D[0] = 0;
            p->unk48D[1] = 0;
            p->movementVelocityLocked = 0;
        } else if (p->movementContactFlags & 1) {
            if (p->unk48D[2] & 1) {
                p->unk48D[0] = 0;
                p->unk48D[1] = 0;
                p->movementVelocityLocked = 0;
            } else {
                p->unk48D[2] = Actor_ProbeDiagonalTurnDir(p);
                angle = ((p->unk48D[2] + 5) & 7) * 32;
                p->lockedVelocity.x = scale(gUnk_086149C4[(angle + 64) & 255] * 10);
                p->lockedVelocity.z = scale(gUnk_086149C4[angle] * 10);
            }
        } else if (!p->unk48D[0]) {
            if (p->unk48D[1]) {
                Player_PlayLocalSong(p->slot, 0x335);
                p->unk48D[0] = 1;
            } else {
                p->unk48D[1]++;
            }
        }
    } else if (Player_IsFlaggedWithGlobalModifier08(p->movementStatus)) {
        if ((u8)(p->tetherMode - 1) <= 1 ||
            (p->tetherMode == 6 && (s16)p->directionHistory[0] >= 0)) {
            p->movementVelocityLocked = 1;
            p->unk48D[2] = p->facing;
            angle = ((p->unk48D[2] + 5) & 7) * 32;
            t = gUnk_086149C4;
            p->lockedVelocity.x = scale(t[(angle + 64) & 255] * 10);
            /* reusing the table variable for the result keeps it in r2 */
            t = (const s16 *)scale(t[angle] * 10);
            p->lockedVelocity.z = (s32)t;
        }
    }
}
