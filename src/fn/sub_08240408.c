#include "global.h"
#include "engine.h"
#include "game_state.h"
#include "player.h"

extern const s16 gUnk_086149C4[];

static inline s32 Truncate12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void sub_08238238(struct Player *);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_PlayLocalSong(s32, s32);

void Player_UpdateKnockback(struct Player *p)
{
    if (p->tetherMode != 0x1E) {
        Player_SetTetherMode(p, 0x1E, 0);
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag) {
            Player_PlayLocalSong(p->slot, p->contactActionTag);
            p->contactActionTag = 0;
        }
    }
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x29), 0x40);
            if (++p->tetherFrame > 2)
                Player_SetTetherMode(p, 0x1E, 1);
            break;
        case 1: {
            u32 a;
            if (p->tetherFrame == 0)
                p->terrainRecoveryCounter = 300;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x2B), 0x40);
            a = ((p->facing + 5) & 7) << 5;
            p->velocityX = -Truncate12(gUnk_086149C4[(a + 0x40) & 0xFF] * p->directionFrame);
            p->velocityZ = -Truncate12(gUnk_086149C4[a] * p->directionFrame);
            p->directionFrame = p->directionFrame * 3 >> 2;
            if (p->tetherFrame > 3) {
                u16 *c = &p->terrainRecoveryCounter;
                if (*c >= (s32)(p->tetherFrame << 3))
                    *c = *c - (p->tetherFrame << 3);
                else
                    *c = 0;
            }
            p->tetherFrame++;
            if (p->directionFrame <= 0x13 && p->terrainRecoveryCounter == 0) {
                p->terrainRecoveryCounter = 0;
                Player_PlayLocalSong(p->slot, 0x2A8);
                Player_SetTetherMode(p, 0x1E, 2);
            }
            break;
        }
        case 2: {
            u32 a = ((p->facing + 5) & 7) << 5;
            p->velocityX = -Truncate12(gUnk_086149C4[(a + 0x40) & 0xFF] * p->directionFrame);
            p->velocityZ = -Truncate12(gUnk_086149C4[a] * p->directionFrame);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x2C),
                                         0x40)) {
                p->directionFrame = 0;
                Player_SetTetherMode(p, 0x1E, 3);
            }
            break;
        }
        case 3:
            if (++p->tetherFrame > 0x13)
                Player_SetTetherMode(p, 0x1E, 4);
            break;
        case 4:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x2D),
                                         0x40)) {
                p->contactSpecial = 0;
                sub_08238238(p);
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
}
