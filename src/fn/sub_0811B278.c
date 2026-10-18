#define SpatialIndex_SetCallback     Collider_SetContactCallback
#define SpatialIndex_SetBounds       Collider_SetExtents
#define SpatialIndex_SetOffset       Collider_SetOffset
#define SpatialIndex_SetMask         Collider_SetCategory
#define SpatialIndex_SetKind         Collider_SetContactParameter
#define SpatialIndex_DefaultCallback Collider_OnContactDefault
/* Provisional family and semantic names; inferred from static accesses and callers. */
#include "global.h"
struct SpatialEntry {
    u32 flags, mask, category;
    u8 unknownC[0x25];
    s8 bucket;
    u8 unknown32[4];
    u8 parameter;
};
struct SpatialVector {
    s16 x, y, z, pad;
};
void SpatialIndex_DefaultCallback(void);
void SpatialIndex_SetCallback(struct SpatialEntry *, void (*)(void), void *);
void SpatialIndex_SetPosition(struct SpatialEntry *, s32, s32, s32);
void SpatialIndex_SetBounds(struct SpatialEntry *, s32, s32, s32);
void SpatialIndex_SetOffset(struct SpatialEntry *, struct SpatialVector *);
void SpatialIndex_SetMask(struct SpatialEntry *, u32);
void SpatialIndex_SetKind(struct SpatialEntry *, u32);
void SpatialIndex_InitEntry(struct SpatialEntry *entry, struct SpatialVector *position,
                            struct SpatialVector *bounds, struct SpatialVector *offset, u32 flags,
                            u32 mask, u32 category, u32 kind, u32 parameter, void (*callback)(void),
                            void *owner)
{
    entry->bucket = -1;
    if (!callback)
        SpatialIndex_SetCallback(entry, SpatialIndex_DefaultCallback, owner);
    else
        SpatialIndex_SetCallback(entry, callback, owner);
    SpatialIndex_SetPosition(entry, position->x, position->y, position->z);
    SpatialIndex_SetBounds(entry, bounds->x, bounds->y, bounds->z);
    SpatialIndex_SetOffset(entry, offset);
    entry->flags |= flags;
    SpatialIndex_SetMask(entry, mask);
    entry->parameter = parameter;
    entry->category = category;
    SpatialIndex_SetKind(entry, kind);
}
