#include "global.h"
#include "monsters/screamer/screamer_playback.h"
#include "engine/engine.h"

void Collider_SetBodyProperties(void *, u32, u32, u32);
void Screamer_OnColliderContact(void);
void Screamer_ApplyHitReaction(void);

void Screamer_InitColliders(struct ScreamerState *screamer)
{
    struct ContactCollider *collider = &screamer->attackCollider;
    struct ContactPosition bounds, offset;

    if (!screamer->f1D8) {
        bounds.x = 160;
        bounds.y = 128;
        bounds.z = 128;
        offset.x = 128;
        offset.y = 0;
        offset.z = 0;
    } else {
        bounds.x = 128;
        bounds.y = 128;
        bounds.z = 160;
        offset.x = 0;
        offset.y = 0;
        offset.z = 128;
    }
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &bounds, &offset);
    Collider_SetCallback(collider, Screamer_OnColliderContact, screamer);
    Collider_SetAttackProperties(collider, screamer->f1C4, screamer->f1C6, 0, 0, screamer->f1C8);

    collider = &screamer->bodyCollider;
    bounds.x = 32;
    bounds.y = 128;
    bounds.z = 32;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &bounds, &offset);
    Collider_SetPosition(collider, &screamer->sprite.position, 0);
    Collider_SetCallback(collider, Screamer_ApplyHitReaction, screamer);
    Collider_SetBodyProperties(collider, 0, (u16)screamer->f1E8, (u16)screamer->f1EA);
    Collider_Refresh(collider);
    collider->flags |= 4;

    collider = &screamer->frontCollider;
    if (!screamer->f1D8) {
        bounds.x = 150;
        bounds.y = 120;
        bounds.z = 120;
        offset.x = 128;
        offset.y = 0;
        offset.z = 0;
    } else {
        bounds.x = 120;
        bounds.y = 120;
        bounds.z = 150;
        offset.x = 0;
        offset.y = 0;
        offset.z = 128;
    }
    Collider_Init(collider, 0, 0x4009, 0, 0x10, &bounds, &offset);
    Collider_SetPosition(collider, &screamer->sprite.position, 0);
    Collider_SetCallback(collider, Screamer_ApplyHitReaction, screamer);
    Collider_SetBodyProperties(collider, 0, (u16)screamer->f1E8, (u16)screamer->f1EA);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
