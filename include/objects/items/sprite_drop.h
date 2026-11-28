#ifndef GUARD_INCLUDE_OBJECTS_ITEMS_SPRITE_DROP_H
#define GUARD_INCLUDE_OBJECTS_ITEMS_SPRITE_DROP_H

#include "global.h"
#include "engine/engine.h"
#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"
#include "scene/collision/spatial_entry.h"

#define SPRITE_DROP_COUNT 16

/* A pooled sprite dropped into the scene. Field names are provisional. */
struct SpriteDrop {
    u8 graphics[0x1C];                    /* 0x00: animation data */
    struct DisplaySequenceSprite sprite;  /* 0x1C */
    struct DisplaySequenceState sequence; /* 0x48 */
    void *sequences;                      /* 0x58 */
    struct SpatialEntry pickup;           /* 0x5C */
    s16 x, y, z;                          /* 0x9C */
    u8 unknownA2[0xC];
    u16 kind;   /* 0xAE */
    u8 active;  /* 0xB0 */
    u8 entered; /* 0xB1 */
    u8 unknownB2[2];
    void (*update)(struct SpriteDrop *);
};

struct SpriteDropPool {
    struct Task task;
    struct SpriteDrop items[SPRITE_DROP_COUNT];
};

typedef char SpriteDropSizeCheck[sizeof(struct SpriteDrop) == 0xB8 ? 1 : -1];
typedef char SpriteDropPoolSizeCheck[sizeof(struct SpriteDropPool) == 0xB98 ? 1 : -1];

struct SpriteDropPool *SpriteDrop_CreateManager(void);
s32 ActorParticles_InitCompactPool(struct SpriteDropPool *);
s32 CallbackPool_InvokeActiveSmall(struct SpriteDropPool *);
s32 CallbackPool_RemoveSmallSprites(struct SpriteDropPool *);
void SpriteDrop_UpdateEntry(struct SpriteDrop *);
void SpriteDrop_OnDeactivate(struct SpriteDrop *);
void SpriteDrop_OnExpire(void *owner, struct SpatialEntry *other);
u8 SpriteDrop_IsNearPlayer(struct SpriteDrop *);

#endif
