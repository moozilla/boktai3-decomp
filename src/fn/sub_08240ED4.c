#include "global.h"
#include "player.h"
void Player_SetTetherMode(struct Player *, u32, u32);
void Player_PlayLocalSong(s32, u32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
void Player_SetDirectionalVelocity_08238300(struct Player *, s32);
void sub_08238238(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_UpdateStumble(struct Player *p)
{
    u16 *frame;
    if (p->tetherMode != 2) {
        Player_SetTetherMode(p, 2, 0);
        Player_PlayLocalSong(p->slot, 0x344);
    }
    Player_SetSecondaryActionFlags(p, 8);
    frame = &p->tetherFrame;
    (*frame)++;
    if (*frame > 0xF) {
        Player_SetTetherMode(p, 0, 0);
        return;
    }
    Player_SetSecondaryActionFlags(p, 2);
    if ((u16)(*frame - 5) <= 7)
        Player_SetSecondaryActionFlags(p, 1);
    Player_SetSecondaryActionFlags(p, 4);
    p->actionEighteenParameter = 0x174;
    p->actionEighteenDuration = 0x18;
    if (p->movementVelocityLocked == 0)
        Player_SetDirectionalVelocity_08238300(p, 0x20);
    sub_08238238(p);
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x40);
}
