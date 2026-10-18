#define CannonBattery_InitMuzzleFlash sub_0819A110
#include "global.h"
#include "engine.h"
struct CannonBatteryPosition {
    s16 x, y, z, unk6;
};
struct CannonBatteryMuzzleFlash {
    u32 update, destroy;
    u8 animation[0x1C], sprite[0x1C];
    struct CannonBatteryPosition position;
    u8 unk48[8], sequence[0x10];
    u8 unk60[0x57], unkB7;
};
u32 CannonBattery_LoadAnimation(void);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void CannonBattery_UpdateMuzzleFlash(void);
void CannonBattery_DestroyMuzzleFlash(void);
void sub_0819A110(void *pool, struct CannonBatteryMuzzleFlash *p,
                  struct CannonBatteryPosition *origin, s32 horizontal)
{
    void *resource = (void *)CannonBattery_LoadAnimation();
    AnimData_Load(p->animation, 0x8D95);
    AnimData_SelectPaletteBank(p->animation, 0x8F);
    Sprite_Init(p->sprite, p->animation, 0);
    if (horizontal) {
        Anim_RestartSequence(p->sequence, resource, 0, 0, 9);
    } else {
        Anim_RestartSequence(p->sequence, resource, 0, 0, 8);
    }
    if (horizontal) {
        p->position.x = origin->x - 0xC0;
        p->position.y = origin->y + 0x80;
        p->position.z = origin->z;
    } else {
        p->position.x = origin->x;
        p->position.y = origin->y + 0x80;
        p->position.z = origin->z - 0xC0;
    }
    p->unkB7 = 0;
    {
        u32 update = (u32)CannonBattery_UpdateMuzzleFlash;
        u32 destroy = (u32)CannonBattery_DestroyMuzzleFlash;
        p->update = update;
        p->destroy = destroy;
    }
}
