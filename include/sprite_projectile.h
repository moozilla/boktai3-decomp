#ifndef GUARD_SPRITE_PROJECTILE_H
#define GUARD_SPRITE_PROJECTILE_H

#include "sprite.h"
#include "display_sequence.h"
#include "task.h"

struct Projectile {
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    struct SpriteRenderResource graphics;
    u8 collider[0x54];
    void *sequenceResource;
    struct ContactPosition velocity;
    u8 unknownB8;
    s8 spin;
    u8 spinScale, colliderFlags;
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[2];
    u32 resourceId;
    u8 enter, soundDelay;
    u16 lifetime, cell;
    u8 unknownCE[6];
};
struct ProjectilePool {
    struct Task task;
    struct Projectile entries[10];
    u32 allocated;
};

#endif
