#include "global.h"
#include "player.h"
void Player_SetTetherMode(struct Player *, u32, u32);
void Player_PlayLocalSong(s32, u32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_SetAnimationIndex(struct Player *, s32, s32);
void SelectionSlots_ClearValue(void *, u32);
void sub_0823C280(struct Player *);
#define F(T, off) (*(T *)((u8 *)p + (off)))
void Player_UpdateDefeatSequence(struct Player *p)
{
    s32 phase;
    Player_SetSecondaryActionFlags(p, 4);
    phase = p->tetherPhase;
    switch (phase) {
        case 0:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40);
            F(u16, 0x4E8) = phase;
            F(u32, 0x40) = phase;
            SelectionSlots_ClearValue((u8 *)p + 0x30, p->slot);
            Player_PlayLocalSong(p->slot, 0x1AD);
            Player_SetTetherMode(p, 0x21, 1);
            break;
        case 1:
            p->tetherFrame++;
            if (p->tetherFrame > 5) {
                F(u16, 0x4E8) = 0;
                Player_PlayLocalSong(p->slot, 0x1AF);
                Player_SetTetherMode(p, 0x21, 2);
            }
            break;
        case 2: {
            u16 *counter;
            s32 zero;
            Player_SetAnimationIndex(p, 0x2C, 0x40);
            counter = &F(u16, 0x4E8);
            {
                u16 t = *counter - 0x1E;
                zero = 0;
                *counter = t;
            }
            p->tetherFrame++;
            if (p->tetherFrame > 8) {
                p->currentStatA = zero;
                *counter = zero;
                Player_SetSecondaryActionFlags(p, 1);
                Player_SetTetherMode(p, 0x21, 3);
            }
            break;
        }
        case 3:
            Player_SetSecondaryActionFlags(p, 1);
            p->tetherFrame++;
            if (p->tetherFrame > 0x3B)
                sub_0823C280(p);
            break;
    }
}
