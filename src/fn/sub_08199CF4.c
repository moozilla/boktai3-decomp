#include "global.h"
#include "engine.h"
struct CannonBatteryPosition {
    s16 x, y, z, unk6;
};
struct CannonBatterySprite {
    u32 flags;
    u8 unk4[3];
    u8 mode;
    u8 unk8[8];
    u16 frame;
    u8 unk12[0xA];
};
struct CannonBatteryCannon {
    u32 update, destroy;
    u8 animation[0x1C];
    struct CannonBatterySprite sprite;
    struct CannonBatteryPosition position;
    u8 unk48[8];
    u8 collider[0x54];
    struct CannonBatteryPosition initialPosition;
    u8 unkAC[0x24];
    u16 unkD0;
    u8 unkD2;
    u8 state;
};
u32 CannonBattery_LoadAnimation(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void CallbackSlot_StartWhenTimerReady(void);
void CannonBattery_UpdateCannon(void *);
void CannonBattery_DestroyCannon(void *);
extern u8 gUnk_0824F6D8[], gUnk_0824F6E0[];
void CannonBattery_InitCannon(struct CannonBatteryCannon *p, s32 x, s32 y, s32 z, s32 flip)
{
    struct CannonBatterySprite *sprite;
    CannonBattery_LoadAnimation();
    AnimData_Load(p->animation, 0x8D95);
    AnimData_SelectPaletteBank(p->animation, 0x8F);
    sprite = &p->sprite;
    Sprite_Init(sprite, p->animation, 0);
    p->position.x = x;
    p->position.y = y;
    p->position.z = z;
    p->initialPosition = p->position;
    p->sprite.mode = 2;
    sprite->frame = 0;
    p->state = 0;
    p->unkD2 = 0;
    p->unkD0 = 0;
    if (flip)
        p->sprite.flags |= 4;
    else
        p->sprite.flags &= ~4;
    Collider_Init(p->collider, 0, 0x4101, 0, 0x10, gUnk_0824F6D8, gUnk_0824F6E0);
    Collider_SetPosition(p->collider, &p->position, 0);
    Collider_SetBodyProperties(p->collider, 0, 0x100, 0);
    Collider_SetCallback(p->collider, CallbackSlot_StartWhenTimerReady, p);
    Collider_Refresh(p->collider);
    {
        u32 update = (u32)CannonBattery_UpdateCannon;
        u32 destroy = (u32)CannonBattery_DestroyCannon;
        p->update = update;
        p->destroy = destroy;
    }
}
