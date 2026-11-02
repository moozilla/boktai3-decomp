#ifndef GUARD_PAIRED_SCENE_ACTOR_H
#define GUARD_PAIRED_SCENE_ACTOR_H

#include "contact_swarm_sprite.h"
#include "motion_track.h"
#include "scene_input_widget.h"

struct PairedHalfGraphics {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    u16 *palette;
    u8 unknown10[0x1C];
};
struct PairedSceneHalf {
    struct ContactSwarmSprite sprite;
    struct PairedHalfGraphics graphics;
    struct MotionTrack motion;
    void *origin;
    u8 unknown94[8];
    struct ContactPosition offset;
    u8 unknownA4[8];
    u16 heading, scale, fieldB0, fieldB2;
    u8 unknownB4[0x54];
    u8 damageCollider[0x54];
    s32 mode;
    u32 actionFlags, actionCounters[2];
    u8 state, requestedState, entered, unknown16F, finished, unknown171;
    u16 active;
    s16 health, maxHealth;
    u32 stateTimer, hitFlags;
    u16 damage, specialDamage, flashTimer;
    u8 field186, field187;
    u16 field188;
    u8 unknown18A[6];
    u16 *palettes[3];
    u32 shake;
    u8 shadow[0x6C];
    u8 target[0x10];
};
struct VanargandAttackPair {
    u8 task[0x18];
    struct SceneInputWidget widget;
    struct SceneInputResource resource;
    struct PairedSceneHalf halves[2];
    const void *effectResource;
    u16 palettes[7][16];
    u32 selectedMask;
};
static inline u8 PairedSceneHalf_IsAlive(const struct PairedSceneHalf *half)
{
    u8 alive = 0;
    if (half->health > 0)
        alive = 1;
    return alive;
}
s32 PairedSceneActor_ApplyToSelectedHalves(struct VanargandAttackPair *, u32 mask, u32 value);
u32 PairedSceneActor_AreSelectedHalvesFinished(struct VanargandAttackPair *);
u32 ScenePair_CountPositiveCounters(struct VanargandAttackPair *);
void Vanargand_ResetState(struct PairedSceneHalf *, s32);
void PairedSceneActor_InitializeHalf(struct PairedSceneHalf *, void *, s32);
s32 sub_081B5EA4(struct VanargandAttackPair *, void *origin, u16 (*palettes)[16], s32 mode);

typedef char PairedSceneHalfSizeCheck[sizeof(struct PairedSceneHalf) == 0x21c ? 1 : -1];
typedef char VanargandAttackPairSizeCheck[sizeof(struct VanargandAttackPair) == 0x5b8 ? 1 : -1];

#endif
