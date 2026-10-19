#include "global.h"
#define ActorTrail_AdvancePositions sub_080A9988
s32 Mod(s32, s32);
struct TrailPosition { s16 x, y, z, w; };
struct ActorTrailSegment {
    struct TrailPosition position;
    u8 unknown8[0xE1];
    u8 flags;
    u8 unknownEA[0xE];
    struct TrailPosition previousPosition;
    u8 unknown100[0x28];
};
struct ActorTrail {
    u8 unknown0[4];
    struct { u8 unknown0[0x1C]; struct TrailPosition position; } *transform;
    u8 unknown8[0x94];
    s16 health;
    u8 unknown9E[0x22C];
    u16 tick;
    u8 unknown2CC[4];
    u32 flags;
    u16 stateFlags;
    u8 unknown2D6[0xFA];
    struct ActorTrail *trail;
    u8 unknown3D4[0x1C];
    struct ActorTrailSegment segments[3];
    u8 unknown768[0x10C];
    u8 interval;
    u8 unknown875[0x65];
    s16 segmentCount;
};
static inline u32 mask16(struct ActorTrail *p, u32 mask) { return p->stateFlags & mask; }
static inline u32 mask32(struct ActorTrail *p, u32 mask) { return p->flags & mask; }
static inline u8 unavailable(struct ActorTrail *p) {
    if (p && (mask16(p, 0x40) || mask32(p, 0x20000) || p->health <= 0)) return 1;
    return 0;
}
static inline u8 flag(u8 *p, u32 mask) { if (*p & mask) return 1; return 0; }
void sub_080A9988(struct ActorTrail *p) {
    struct ActorTrail *trail;
    struct TrailPosition position, old;
    s32 i;
    if (unavailable(p)) return;
    trail = p->trail;
    if (Mod(p->tick, trail->interval)) return;
    position = p->transform->position;
    for (i = 0; i < trail->segmentCount; i++) {
        if (flag(&trail->segments[i].flags, 2) && !flag(&trail->segments[i].flags, 1)) {
            old = trail->segments[i].position;
            trail->segments[i].previousPosition = position;
            position = old;
        }
    }
}
