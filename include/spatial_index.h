#ifndef GUARD_SPATIAL_INDEX_H
#define GUARD_SPATIAL_INDEX_H

#include "engine.h"
#include "spatial_entry.h"
#include "tilemap_panel.h"
#include "background_controller.h"

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

void Collider_ScheduleUpdates(void *, struct SpatialBucket *, s32 bucketIndex);
void Collider_OnContactDefault(void *, struct SpatialEntry *);
void Collider_SetContactCallback(struct SpatialEntry *, SpatialContactCallback callback,
                                 void *owner);
void Collider_SetExtents(struct SpatialEntry *, s32 x, s32 y, s32 z);
void SpatialIndex_SetPosition(struct SpatialEntry *, s32 x, s32 y, s32 z);
void Collider_SetFixedPositionWithYOffset(struct SpatialEntry *, u32 position, u32 yOffset);
void Collider_SetOffset(struct SpatialEntry *, const struct ContactPosition *);
void Collider_SetCategory(struct SpatialEntry *, u32 mask);
void Collider_SetContactParameter(struct SpatialEntry *, u32 kind);
void SpatialIndex_InitEntry(struct SpatialEntry *, struct ContactPosition *,
                            struct ContactPosition *, struct ContactPosition *, u32 flags, u32 mask,
                            u32 category, u32 kind, u32 parameter, SpatialContactCallback callback,
                            void *owner);
void SpatialIndex_InsertEntry(struct SpatialEntry *);
u8 SpatialIndex_RemoveEntry(struct SpatialEntry *);

s32 Div(s32, s32);
void Position_GetHeightAdjustedCoordinates(u32, u32, s16 *);

#endif
