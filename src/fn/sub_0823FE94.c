#include "global.h"
#include "player.h"
void Player_SetTetherMode(struct Player *, u32, u32);
void Player_PlayLocalSong(s32, u32);
u32 LinkPlayer_HasSlotValueAtLeast(struct Player *, s32);
void *SolarBomb_Create(struct Player *, struct SceneVec *, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_UpdateSolarBombCharge(struct Player *p)
{
    if (p->tetherMode != 0x12) {
        if (p->tetherStatusFlags != 0) {
            p->actionEighteenEnabled = 0;
        } else {
            u32 ready = LinkPlayer_HasSlotValueAtLeast(p, 3);
            if (ready) {
                p->actionEighteenEnabled = 1;
                SolarBomb_Create(p, &p->position, 0);
            } else {
                p->actionEighteenEnabled = ready;
            }
        }
        Player_SetTetherMode(p, 0x12, 0);
    }
    switch (p->tetherPhase) {
        case 0: {
            u8 *enabled = &p->actionEighteenEnabled;
            if (*enabled != 0) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 0x20;
            }
            if (Player_SetAnimationIndex(p, 0x8B, 0x40) != 0) {
                if (*enabled != 0)
                    Player_SetTetherMode(p, 0x12, 1);
                else
                    Player_SetTetherMode(p, 0x12, 2);
            }
            break;
        }
        case 1:
            if (p->actionEighteenEnabled != 0) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 0x20;
            }
            p->tetherFrame++;
            if (p->tetherFrame > 0xB)
                Player_SetTetherMode(p, 0x12, 3);
            break;
        case 2: {
            u16 *frame = &p->tetherFrame;
            if (*frame == 0)
                Player_PlayLocalSong(p->slot, 0x192);
            Player_SetAnimationIndex(p, 0x95, 0x40);
            (*frame)++;
            if (*frame > 0xB)
                Player_SetTetherMode(p, 0x12, 3);
            break;
        }
        case 3:
            if (Player_SetAnimationIndex(p, 0x90, 0x40) != 0)
                Player_SetTetherMode(p, 0, 1);
            break;
    }
}
