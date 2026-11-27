#include "bosses/hresvelgr/hresvelgr_shot.h"

struct HresvelgrShotPool {
    u8 unknown00[0x18];
    u8 animationData[0x28];
    s16 attackA, attackB, attackC;
    u8 unknown46[2];
    struct HresvelgrShot shots[32];
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
s32 Hresvelgr_InitializeShots(struct HresvelgrShotPool *pool)
{
    struct HresvelgrShot *shot;
    s32 i;
    struct DisplaySequenceSprite *sprite;
    u16 zero, x, y, z;
    AnimData_Load(pool->animationData, 0x3070);
    AnimData_SelectPaletteBank(pool->animationData, 0x2F4);
    shot = pool->shots;
    zero = 0;
    i = 31;
    sprite = &shot->sprite;
    x = 0x708;
    y = 0x100;
    z = 0x4B0;
    do {
        Sprite_Init(shot, pool->animationData, 0);
        sprite->frame = zero;
        sprite->position.x = x;
        sprite->position.y = y;
        sprite->position.z = z;
        sprite->flags |= 1;
        shot->lifetime = zero;
        Hresvelgr_InitShotCollider(shot, pool->attackA, pool->attackB, pool->attackC);
        sprite = (struct DisplaySequenceSprite *)((u8 *)sprite + sizeof(*shot));
        shot++;
    } while (--i >= 0);
    return 0;
}
