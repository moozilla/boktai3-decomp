#include "global.h"


extern const s16 gUnk_086149C4[];

struct TrailPosition {
    u16 x, y, z, unk_06;
};
struct TrailEntry {
    u32 flags;
    u8 unk_04[0x14];
    struct TrailPosition position;
    u8 unk_20[8];
    u8 active;
    s8 angleOffset;
    u16 radius;
    u16 timer;
    u8 unk_2E[2];
};
/* A position-first view of an entry, retaining the sprite array stride. */
struct TrailMotion {
    struct TrailPosition position;
    u8 unk_08[8];
    u8 active;
    s8 angleOffset;
    u16 radius;
    u16 timer;
    u8 unk_16[0x1A];
};
struct TrailGroup {
    u8 unk_00[0x48];
    struct TrailEntry entries[4];
};
struct TrailSource {
    u8 unk_00[0x1E];
    s8 direction;
    u8 unk_1F[0xA8 - 0x1F];
    struct TrailPosition position;
};
struct PositionWords {
    u32 a, b;
};
static inline s32 Truncate(s32 product)
{
    if (product >= 0)
        return product >> 12;
    return -((-product) >> 12);
}

// Tentative: shrink the four radial trail sprites, hiding each after its fifteenth tick.
void Task4E01_UpdateTrailSprites(struct TrailGroup *group, struct TrailSource *source)
{
    s32 direction;
    u8 facing;
    s32 remaining;
    s32 byteOffset;
    struct TrailMotion *motion;
    s32 angleMask;
    u16 *age;
    direction = (facing = ((u16)source->direction + 0x70) >> 5) & 7;
    direction = ((direction + 5) & 7) << 5;
    angleMask = 255;
    byteOffset = 0;
    motion = (struct TrailMotion *)&group->entries[0].position;
    age = &motion->timer;
    remaining = 3;
    do {
        if (motion->active) {
            if (++motion->timer > 14) {
                struct TrailEntry *sprite;
                motion->active = 0;
                sprite = group->entries;
                sprite = (struct TrailEntry *)((u8 *)sprite + byteOffset);
                sprite->flags |= 1;
            } else {
                s32 radius;
                s32 angle;
                s32 offset;
                struct PositionWords *out;
                radius = (motion->radius * (16 - *age)) >> 3;
                offset = motion->angleOffset + 256;
                angle = (direction + offset) & angleMask;
                out = (struct PositionWords *)&group->entries[0].position;
                out = (struct PositionWords *)((u8 *)out + byteOffset);
                *out = *(struct PositionWords *)&source->position;
                motion->position.x += Truncate(gUnk_086149C4[(angle + 64) & angleMask] * radius);
                motion->position.z += Truncate(gUnk_086149C4[angle & angleMask] * radius);
            }
        }
        byteOffset += sizeof(struct TrailEntry);
        motion++;
        age += sizeof(struct TrailEntry) / sizeof(u16);
    } while (--remaining >= 0);
}
