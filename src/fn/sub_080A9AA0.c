#include "global.h"
#define gTrailSineTable gUnk_086149C4
extern const s16 gUnk_086149C4[];
struct TrailOscillator {
    u8 unknown0[0xE9];
    u8 flags;
    u8 unknownEA[4];
    u8 angle;
    u8 unknownEF[0x11];
    u16 offsetX, offsetY, offsetZ;
    u8 unknown106[0x22];
};
struct ActorTrail {
    u8 unknown0[0x3F0];
    struct TrailOscillator segments[3];
    u8 unknown768[0x172];
    s16 segmentCount;
};
struct TrailActor { u8 unknown0[0x3D0]; struct ActorTrail *trail; };
static inline u8 isActive(struct TrailOscillator *segment) {
    u32 value = segment->flags;
    u32 mask = 2;
    if (mask & value) return 1;
    return 0;
}
static inline u8 isHidden(struct TrailOscillator *segment) {
    u32 value = segment->flags;
    u32 mask = 1;
    if (mask & value) return 1;
    return 0;
}
static inline s32 fixedMultiply(s32 a, s32 b) {
    s32 product = a * b;
    if (product >= 0) return product >> 12;
    return -((-product) >> 12);
}
void ActorTrail_ApplyAngularOffsets(struct TrailActor *actor, s32 magnitude, s32 angleStep, s32 mode) {
    struct ActorTrail *trail = actor->trail;
    s32 i;
    for (i = 0; i < trail->segmentCount; i++) {
        if (isActive(&trail->segments[i]) && !isHidden(&trail->segments[i])) {
            if (mode == 0) {
                trail->segments[i].offsetY += fixedMultiply(gTrailSineTable[trail->segments[i].angle], magnitude);
            } else {
                trail->segments[i].offsetX += fixedMultiply(gTrailSineTable[(trail->segments[i].angle + 0x40) & 0xFF], magnitude);
                trail->segments[i].offsetZ += fixedMultiply(gTrailSineTable[trail->segments[i].angle], magnitude);
            }
            trail->segments[i].angle += angleStep;
        }
    }
}
