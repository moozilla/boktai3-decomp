#include "global.h"
#include "engine.h"
#define BossAttack_ActivateCollider sub_081905F0
struct BossAttack {
    u8 unknown0[0x4C];
    u32 colliderId;
    u8 unknown50[0x5DFC - 0x50];
    u8 collider[0x724C - 0x5DFC];
    u8 activeTimer;
    u8 unknown724D[0x726E - 0x724D];
    u16 strength;
};
extern u8 gUnk_0824F2AC[];
extern u8 gUnk_0824F2B4[];
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_081905F0(struct BossAttack *boss)
{
    void *collider;
    boss->activeTimer = 20;
    collider = boss->collider;
    Collider_Init(collider, boss->colliderId, 0x2001, 0, 0x10,
                  gUnk_0824F2AC, gUnk_0824F2B4);
    Collider_SetAttackProperties(collider, boss->strength * 5, 200, 0, 0x40000, 30);
    Collider_SetCallback(collider, 0, 0);
    Sound_PlaySongChecked(0x1BA);
}
