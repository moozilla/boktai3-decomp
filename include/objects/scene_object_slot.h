#ifndef SCENE_OBJECT_SLOT_H
#define SCENE_OBJECT_SLOT_H
#include "engine/video/sprite.h"
#include "scene/terrain/terrain_cell_probe.h"
/* Layout of one 0x8C-byte slot of the scene-object pool built by SlidingDoors_Append (confidence=low). */
struct Pair {
    u32 a, b;
};
/* A position, also copied as a pair of words. */
union SceneObjPos {
    struct Pair words;
    struct Pos16 position;
    struct ContactPosition signedPosition;
};
/* Four script argument words. The copy in SlidingDoors_Append goes through the union: its stores
   may alias the pool's u16 count, so gcse leaves the count's load after the copy loop. */
union SceneObjArgs {
    s32 words[4];
    u8 bytes[16];
};
struct SceneObjSlot {
    struct DisplaySequenceSprite sprite;
    struct TerrainCellOverride *terrain;
    void *callback;
    u16 id;
    u8 activationFlags;
    u8 variant;
    u8 dir;
    u8 pose, poseFrame, cellHeight;
    u16 cellA, cellB;
    u8 type;
    u8 autoActivate; /* tentative: enables proximity activation */
    u16 region; /* Region_FindContaining(position) */
    u8 hideOutsideRegion; /* tentative */
    u8 paletteBank;
    u8 pad46[2];
    s32 interactionScript; /* optional script bound to the interaction arguments */
    s32 animationIndex;
    union SceneObjArgs args;
    s32 raisedScript, loweredScript;
    union SceneObjPos pos, q0, r0; /* position, display and base positions */
    u16 lift, timer;
    u32 loweredPose;
    u16 maxLift;
    u8 pad8A[2];
};

typedef char SceneObjSlotSizeCheck[sizeof(struct SceneObjSlot) == 0x8c ? 1 : -1];

enum SceneObjActivationState {
    SCENE_OBJ_INACTIVE,
    SCENE_OBJ_ACTIVE,
    SCENE_OBJ_ACTIVATING,
    SCENE_OBJ_DEACTIVATING
};

enum SceneObjLiftPosition {
    SCENE_OBJ_LIFT_LOWERED,
    SCENE_OBJ_LIFT_RAISED
};

/* The script position and the slot's idle lift variants use opposite values. */
static inline u8 SceneObjSlot_LiftVariant(enum SceneObjLiftPosition position)
{
    return position == SCENE_OBJ_LIFT_LOWERED;
}

static inline u8 SceneObjSlot_LiftMotionVariant(enum SceneObjLiftPosition target)
{
    return target == SCENE_OBJ_LIFT_RAISED ? SCENE_OBJ_ACTIVATING : SCENE_OBJ_DEACTIVATING;
}

/* Start a sprite pose at the first frame of its animation. */
static inline void SceneObjSlot_SetAnimation(struct SceneObjSlot *slot,
                                             enum SceneObjActivationState state,
                                             void (*callback)(struct SceneObjSlot *), u8 pose)
{
    slot->variant = state;
    slot->callback = callback;
    slot->pose = pose;
    slot->poseFrame = 0;
}

#endif
