#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void *DeckCannon_LoadAnimation(void);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void DeckCannon_OnProjectileCollision(struct ContactCollider *, struct ContactCollider *, void *);
void DeckCannon_DestroyProjectile(struct DeckCannonShot *);
void DeckCannon_UpdateProjectile(struct DeckCannonShot *);
extern struct ContactPosition gUnk_0824F6E8, gUnk_0824F6F0;

void DeckCannon_InitProjectile(struct DeckCannonBattery *battery, struct DeckCannonShot *shot,
                               struct ContactPosition *origin, s32 horizontal)
{
    void *resource = DeckCannon_LoadAnimation();
    struct ContactCollider *collider;

    AnimData_Load(shot->animation, 0x8D95);
    AnimData_SelectPaletteBank(shot->animation, 0x8F);
    Sprite_Init(&shot->sprite, shot->animation, 0);
    Anim_RestartSequence(&shot->sequence, resource, 1, 0, 8);
    if (horizontal) {
        shot->sprite.position.x = origin->x - 0xC0;
        shot->sprite.position.y = origin->y + 0x80;
        shot->sprite.position.z = origin->z;
    } else {
        shot->sprite.position.x = origin->x;
        shot->sprite.position.y = origin->y + 0x80;
        shot->sprite.position.z = origin->z - 0xC0;
    }
    shot->finished = 0;
    collider = &shot->collider;
    Collider_Init(collider, 0, 0x2103, 0, 1, &gUnk_0824F6E8, &gUnk_0824F6E8);
    Collider_SetAttackProperties(collider, battery->attack, battery->attackFlags, 0x10, 0x40000,
                                 0xA);
    collider->offset = gUnk_0824F6E8;
    collider->bounds = gUnk_0824F6F0;
    if (horizontal) {
        shot->direction = 0x10;
        {
            u32 direction = 0x80;
            shot->collider.direction = direction;
        }
        shot->startCoordinate = shot->sprite.position.x;
    } else {
        shot->direction = 0;
        {
            u32 direction = 0xC0;
            shot->collider.direction = direction;
        }
        shot->startCoordinate = shot->sprite.position.z;
    }
    Collider_SetCallback(&shot->collider, DeckCannon_OnProjectileCollision, shot);
    {
        void (*update)(struct DeckCannonShot *) = DeckCannon_UpdateProjectile;
        void (*destroy)(struct DeckCannonShot *) = DeckCannon_DestroyProjectile;
        shot->update = update;
        shot->destroy = destroy;
    }
}
