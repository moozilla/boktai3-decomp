#include "global.h"
#include "engine.h"

#define gCannonBatteryProjectileSize   gUnk_0824F6E8
#define gCannonBatteryProjectileOffset gUnk_0824F6F0
struct CannonBatteryPosition {
    s16 x, y, z, unk6;
};
struct CannonBatteryCollider {
    u8 unk0[0x14];
    struct CannonBatteryPosition size, offset;
    u8 unk24[0x1E];
    u8 direction;
    u8 unk43[0x11];
};
struct CannonBatteryProjectile {
    u32 update, destroy;
    u8 animation[0x1C], sprite[0x1C];
    struct CannonBatteryPosition position;
    u8 unk48[8], sequence[0x10];
    struct CannonBatteryCollider collider;
    u16 startCoordinate;
    u8 direction, unkB7;
};
struct CannonBatteryPool {
    u8 unk0[0x20];
    u16 attack, attackFlags;
};
u32 CannonBattery_LoadAnimation(void);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void CannonBattery_OnProjectileCollision(void);
void CannonBattery_DestroyProjectile(void);
void CannonBattery_UpdateProjectile(void);
extern struct CannonBatteryPosition gUnk_0824F6E8, gUnk_0824F6F0;
void CannonBattery_InitProjectile(struct CannonBatteryPool *pool, struct CannonBatteryProjectile *p,
                                  struct CannonBatteryPosition *origin, s32 horizontal)
{
    void *resource = (void *)CannonBattery_LoadAnimation();
    struct CannonBatteryCollider *collider;
    AnimData_Load(p->animation, 0x8D95);
    AnimData_SelectPaletteBank(p->animation, 0x8F);
    Sprite_Init(p->sprite, p->animation, 0);
    Anim_RestartSequence(p->sequence, resource, 1, 0, 8);
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
    collider = &p->collider;
    Collider_Init(collider, 0, 0x2103, 0, 1, &gUnk_0824F6E8, &gUnk_0824F6E8);
    Collider_SetAttackProperties(collider, pool->attack, pool->attackFlags, 0x10, 0x40000, 0xA);
    collider->size = gUnk_0824F6E8;
    collider->offset = gUnk_0824F6F0;
    if (horizontal) {
        p->direction = 0x10;
        {
            u32 direction = 0x80;
            p->collider.direction = direction;
        }
        p->startCoordinate = p->position.x;
    } else {
        p->direction = 0;
        {
            u32 direction = 0xC0;
            p->collider.direction = direction;
        }
        p->startCoordinate = p->position.z;
    }
    Collider_SetCallback(&p->collider, CannonBattery_OnProjectileCollision, p);
    {
        u32 update = (u32)CannonBattery_UpdateProjectile;
        u32 destroy = (u32)CannonBattery_DestroyProjectile;
        p->update = update;
        p->destroy = destroy;
    }
}
