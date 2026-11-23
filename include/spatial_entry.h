#ifndef GUARD_SPATIAL_ENTRY_H
#define GUARD_SPATIAL_ENTRY_H

#include "contact_position.h"

struct SpatialEntry;
typedef void (*SpatialContactCallback)(void *owner, struct SpatialEntry *other);

struct SpatialEntry {
    u32 flags, mask, category; /* 0x00 */
    u16 kind;                  /* 0x0C */
    u8 hit, registrationFlags;
    struct ContactPosition position; /* 0x10 */
    struct ContactPosition extents;  /* 0x18 */
    struct ContactPosition offset;   /* 0x20 */
    struct ContactPosition screen;   /* 0x28 */
    u8 unknown30;
    s8 bucket;
    s16 hitTimer;
    s16 updateDelay; /* 0x34 */
    u8 parameter, unknown37;
    void *owner;                     /* 0x38 */
    SpatialContactCallback callback; /* 0x3C: contact handler */
};

/* Bit 0 marks an entry with updates suspended, bit 1 one skipped this frame. */
static inline u32 SpatialEntry_TestFlags(struct SpatialEntry *entry, u32 mask)
{
    return entry->flags & mask;
}

#endif
