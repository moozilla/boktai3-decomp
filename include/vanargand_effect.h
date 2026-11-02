#ifndef GUARD_VANARGAND_EFFECT_H
#define GUARD_VANARGAND_EFFECT_H

#include "motion_track.h"

struct VanargandEffectSprite {
    u32 flags;
    u8 linked;
    u8 unknown5[0xb];
    u16 frame;
    u8 unknown12[0xa];
    struct MotionTrackVector position;
    u8 unknown24[8];
};

struct VanargandEffect {
    u8 task[0x18];
    struct VanargandEffectSprite sprite;
    u8 graphics[0x1C];
    struct MotionTrack motion;
    u16 delay, lifetime, variant;
};

struct VanargandEffect *VanargandEffect_Create(s32 variant, s32 delay,
                                              const struct MotionTrackVector *position,
                                              const struct MotionTrackVector *target,
                                              u32 alternate);
s32 VanargandEffect_Initialize(struct VanargandEffect *, s32 variant, s32 delay,
                                const struct MotionTrackVector *position,
                                const struct MotionTrackVector *target, u32 alternate);
void VanargandEffect_Update(struct VanargandEffect *);
void VanargandEffect_UpdateLifetime(struct VanargandEffect *);
void VanargandEffect_Destroy(struct VanargandEffect *);
void ActorEffect_UpdateFadeFrame(struct VanargandEffect *);

typedef char VanargandEffectSizeCheck[sizeof(struct VanargandEffect) == 0xA0 ? 1 : -1];

#endif
