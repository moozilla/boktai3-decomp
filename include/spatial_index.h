#ifndef GUARD_SPATIAL_INDEX_H
#define GUARD_SPATIAL_INDEX_H

#include "engine.h"
#include "contact_position.h"
#include "tilemap_panel.h"
#include "background_controller.h"

struct SpatialEntry {
    u32 flags, mask, category; /* 0x00 */
    u16 kind;                  /* 0x0C */
    u8 unknownE, registrationFlags;
    struct ContactPosition position; /* 0x10 */
    struct ContactPosition extents;  /* 0x18 */
    struct ContactPosition offset;   /* 0x20 */
    struct ContactPosition screen;   /* 0x28 */
    u8 unknown30;
    s8 bucket;
    u16 unknown32;
    s16 updateDelay; /* 0x34 */
    u8 parameter, unknown37;
    void *owner;            /* 0x38 */
    void (*callback)(void); /* 0x3C: contact handler; signature provisional */
};

struct SpatialLink {
    struct SpatialEntry *entry;
    struct SpatialLink *previous, *next;
};

struct SpatialBucket {
    struct SpatialLink *active, *passive;
    s32 activeCount, passiveCount;
};

struct SpatialIndex {
    struct Task task;                 /* 0x00 */
    struct SpatialBucket buckets[10]; /* 0x18 */
    u32 unknownB8;
    s32 bucketWidth; /* 0xBC */
};

void Collider_ScheduleUpdates(void *, struct SpatialBucket *);
void Collider_OnContactDefault(void);
void Collider_SetContactCallback(struct SpatialEntry *, void (*callback)(void), void *owner);
void Collider_SetExtents(struct SpatialEntry *, s32 x, s32 y, s32 z);
void SpatialIndex_SetPosition(struct SpatialEntry *, s32 x, s32 y, s32 z);
void Collider_SetFixedPositionWithYOffset(struct SpatialEntry *, u32 position, u32 yOffset);
void Collider_SetOffset(struct SpatialEntry *, const struct ContactPosition *);
void Collider_SetCategory(struct SpatialEntry *, u32 mask);
void Collider_SetContactParameter(struct SpatialEntry *, u32 kind);
void SpatialIndex_InitEntry(struct SpatialEntry *, struct ContactPosition *,
                            struct ContactPosition *, struct ContactPosition *, u32 flags, u32 mask,
                            u32 category, u32 kind, u32 parameter, void (*callback)(void),
                            void *owner);
void SpatialIndex_InsertEntry(struct SpatialEntry *);
u8 SpatialIndex_RemoveEntry(struct SpatialEntry *);

s32 Div(s32, s32);
void Position_GetHeightAdjustedCoordinates(u32, u32, s16 *);

#endif
