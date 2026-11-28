#ifndef GUARD_INCLUDE_OBJECTS_ITEMS_COLLECTIBLE_H
#define GUARD_INCLUDE_OBJECTS_ITEMS_COLLECTIBLE_H

#include "global.h"
#include "engine/engine.h"
#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"
#include "scene/collision/spatial_entry.h"

#define COLLECTIBLE_COUNT 48

/* A pooled collectible dropped into the scene. Field names are provisional. */
struct Collectible {
    u8 graphics[0x1C];                    /* 0x00: animation data */
    struct DisplaySequenceSprite sprite;  /* 0x1C */
    struct DisplaySequenceState sequence; /* 0x48 */
    void *sequences;                      /* 0x58 */
    struct SpatialEntry pickup;           /* 0x5C */
    struct SpatialEntry body;             /* 0x9C: damage received */
    s16 x, y, z;                          /* 0xDC */
    u8 unknownE2[2];
    s16 velocityX, velocityY; /* 0xE4 */
    u8 unknownE8[4];
    s16 health; /* 0xEC */
    u8 type;    /* 0xEE */
    u8 flash;   /* 0xEF */
    u8 active;  /* 0xF0 */
    u8 entered; /* 0xF1 */
    u16 timer;  /* 0xF2 */
    void (*update)(struct Collectible *);
};

struct CollectiblePool {
    struct Task task;
    struct Collectible items[COLLECTIBLE_COUNT];
};

typedef char CollectibleSizeCheck[sizeof(struct Collectible) == 0xF8 ? 1 : -1];
typedef char CollectiblePoolSizeCheck[sizeof(struct CollectiblePool) == 0x2E98 ? 1 : -1];

struct CollectiblePool *Collectible_CreateManager(void);
s32 ActorParticles_InitPool(struct CollectiblePool *);
s32 CallbackPool_InvokeActive(struct CollectiblePool *);
u32 Collectible_DestroyManager(struct CollectiblePool *);
void Collectible_UpdateEntry(struct Collectible *);
void Collectible_StartDeath(struct Collectible *);
void Collectible_OnDeactivate(struct Collectible *);
u8 Collectible_IsNearPlayer(struct Collectible *);
void Collectible_OnPickup(void *owner, struct SpatialEntry *other);
void Collectible_OnContact(void *owner, struct SpatialEntry *other);
void Collectible_ApplyPickupReward(struct Collectible *, struct SpatialEntry *other);
void ActorParticles_SpawnAlternatingBurst(struct Collectible *);

#endif
