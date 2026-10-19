#include "global.h"

#ifdef NONMATCHING
// fuzzy: 91.5%
// foundry draft (worker best 91.5%); what differs is mostly register allocation and instruction order
#define Hresvelgr_InitializeShots sub_081A3554
struct HresvelgrShotSprite {
    u32 flags;
    u8 unknown04[0xC];
    u16 animation;
    u8 unknown12[0xA];
    u16 x, y, z;
};
struct HresvelgrShot {
    struct HresvelgrShotSprite sprite;
    u8 unknown24[0x66];
    u16 lifetime;
    u8 unknown8C[4];
};
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
s32 Hresvelgr_InitShotCollider(struct HresvelgrShot *, s32, s32, s32);
s32 sub_081A3554(struct HresvelgrShotPool *pool)
{
    struct HresvelgrShot *shot;
    s32 i;
    struct HresvelgrShotSprite *sprite;
    u16 zero, y, z;
    AnimData_Load(pool->animationData, 0x3070);
    AnimData_SelectPaletteBank(pool->animationData, 0x2F4);
    shot = pool->shots;
    zero = 0;
    i = 31;
    sprite = &shot->sprite;
    y = 0x100;
    z = 0x4B0;
    do {
        u16 x = 0x708;
        Sprite_Init(shot, pool->animationData, 0);
        sprite->animation = zero;
        sprite->x = x;
        sprite->y = y;
        sprite->z = z;
        sprite->flags |= 1;
        shot->lifetime = zero;
        Hresvelgr_InitShotCollider(shot, pool->attackA, pool->attackB, pool->attackC);
        sprite = (struct HresvelgrShotSprite *)((u8 *)sprite + 0x90);
        shot++;
    } while (--i >= 0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A3554);
#endif
