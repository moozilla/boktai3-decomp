// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
extern const u8 gUnk_08E6290C[];
void Player_InitializeActionState(void *, const void *);
void CellObject_Init(void *, void *);
void sub_08238238(void *);
u32 ActorVariant_GetTableValue(u32, u32);
void Player_SetAnimationIndex(void *, u32, u32);
void Player_SetTetherMode(void *, u32, u32);
#define H(o) (*(u16 *)((u8 *)p + (o)))
#define B(o) (*(u8 *)((u8 *)p + (o)))
// confidence=low: restores saved position and resource values while resetting the linked action state.
#define Player_RestoreLinkedActionState sub_0823C280
void Player_RestoreLinkedActionState(struct Player *p)
{
    u8 *mode = (u8 *)p + 0x418;
    u32 one;
    u32 zeroHalf;
    register u32 zeroByte asm("r9");
    u16 value;
    u32 kind;
    register u16 *destination asm("r0");
    register u32 offset asm("r2");
    void *cell;
    if (*mode != 6)
        Player_InitializeActionState(p, gUnk_08E6290C);
    kind = 1;
    one = kind;
    p->kind = kind;
    offset = 0x426;
    value = *(u16 *)((u8 *)p + offset);
    offset -= 2;
    destination = (u16 *)((u8 *)p + offset);
    zeroByte = 0;
    zeroHalf = 0;
    *destination = value;
    value = H(0x42A);
    H(0x428) = value;
    p->position = *(struct SceneVec *)((u8 *)p + 0x540);
    cell = (u8 *)p + 0x284;
    CellObject_Init(cell, &p->position);
    *(void **)((u8 *)p + 0x40) = cell;
    H(0x26E) = zeroHalf;
    H(0x270) = zeroHalf;
    B(0x479) = zeroByte;
    H(0x274) = 90;
    H(0x4E8) = zeroHalf;
    B(0x54B) = one;
    {
        u32 facing = B(0x548);
        B(0x3A4) = facing;
    }
    sub_08238238(p);
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*mode, 0), 64);
    Player_SetTetherMode(p, 0, 0);
}
