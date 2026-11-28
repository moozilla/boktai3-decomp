#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void ExplosionEffect_Create(struct ContactPosition *, s32, u32, u32, u32);

void DeckCannon_OnProjectileCollision(struct ContactCollider *collider,
                                      struct ContactCollider *other, void *owner)
{
    struct DeckCannonShot *shot = owner;

    ExplosionEffect_Create(&collider->position, 1, collider->unknown3C, collider->responseParameter,
                           0);
    shot->update = 0;
    if (shot->destroy) {
        Callback_InvokeActor(shot, shot->destroy);
        shot->destroy = 0;
    }
}
