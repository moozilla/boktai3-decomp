#include "global.h"
struct VecB278 {
    u32 x : 16, y : 16, z : 16;
};
void Collider_Init(u8 *, s32, s32, s32, s32, struct VecB278 *, struct VecB278 *);
void Collider_SetAttackProperties(u8 *, s32, s32, s32, s32, s32);
void Collider_SetCallback(u8 *, void *, u8 *);
void Collider_SetPosition(u8 *, u8 *, s32);
void Player_ApplyGatedContact(void);
s32 Collider_Refresh(u8 *);
void Player_InitBodyCollider(u8 *p)
{
    struct VecB278 scale;
    struct VecB278 position;
    u8 *q = p + 0x230;
    scale.x = 50;
    scale.y = 127;
    scale.z = 50;
    position.x = 0;
    position.y = 127;
    position.z = 0;
    Collider_Init(q, *(u16 *)(p + 0x24), 0x4001, 0, (u16)(1U << *(u32 *)(p + 0x18)), &scale,
                  &position);
    Collider_SetCallback(q, Player_ApplyGatedContact, p);
    Collider_SetPosition(q, p + 0x168, 0);
    Collider_Refresh(q);
}
