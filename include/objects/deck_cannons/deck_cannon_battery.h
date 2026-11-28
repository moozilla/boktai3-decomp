#ifndef GUARD_OBJECTS_DECK_CANNON_BATTERY_H
#define GUARD_OBJECTS_DECK_CANNON_BATTERY_H

#include "global.h"
#include "engine/engine.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "engine/video/display_sequence.h"
#include "scene/collision/contact_collider.h"
#include "objects/deck_cannons/callback_slot_layout.h"

/* A projectile or muzzle flash. confidence=low */
struct DeckCannonShot {
    void (*update)(struct DeckCannonShot *);
    void (*destroy)(struct DeckCannonShot *);
    u8 animation[0x1C];
    struct ContactSwarmSprite sprite; /* 0x24 */
    struct DisplaySequenceState sequence;
    struct ContactCollider collider; /* 0x60: projectiles only */
    u16 startCoordinate;
    u8 direction;
    u8 finished;
};

struct DeckCannonBattery {
    struct Task task;
    u32 messageId;
    u16 usedCannons, usedShots; /* one bit per slot */
    u16 attack, attackFlags;
    u16 delay;
    struct CallbackSlot cannons[16]; /* 0x28 */
    struct DeckCannonShot shots[8];  /* 0xD68 */
};

typedef char DeckCannonBatterySizeCheck[sizeof(struct DeckCannonBattery) == 0x1328 ? 1 : -1];

#endif
