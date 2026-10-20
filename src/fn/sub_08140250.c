#include "global.h"
// confidence=low: sets up the player body collider that grants stat rewards on contact.
struct Vec40250 {
    u32 x : 16, y : 16, z : 16;
};
void Collider_Init(u8 *, s32, s32, s32, s32, struct Vec40250 *, struct Vec40250 *);
void Collider_SetBodyProperties(u8 *, s32, s32, s32);
void Collider_SetCallback(u8 *, void *, u8 *);
void Collider_SetPosition(u8 *, u8 *, s32);
void PlayerContact_ApplyStatReward(void);
void PlayerAction_RefreshModifiers(u8 *);
s32 Collider_Refresh(u8 *);
void sub_08140250(u8 *p)
{
    struct Vec40250 scale;
    struct Vec40250 position;
    u8 *q = p + 0x230;
    scale.x = 50;
    scale.y = 127;
    scale.z = 50;
    position.x = 0;
    position.y = 127;
    position.z = 0;
    Collider_Init(q, *(u16 *)(p + 0x24), 0x5001, 0, (u16)(1U << *(u32 *)(p + 0x18)), &scale,
                  &position);
    switch (p[0x418]) {
        case 0:
        case 2:
            Collider_SetBodyProperties(q, 0x14, 1, 2);
            break;
        case 1:
        case 6:
            Collider_SetBodyProperties(q, 0x14, 2, 1);
            break;
        default:
            Collider_SetBodyProperties(q, 0x14, 1, 2);
            break;
    }
    Collider_SetCallback(q, PlayerContact_ApplyStatReward, p);
    Collider_SetPosition(q, p + 0x168, 0);
    Collider_Refresh(q);
    PlayerAction_RefreshModifiers(p);
}
