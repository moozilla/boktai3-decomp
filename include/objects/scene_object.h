#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H
#include "engine/engine.h"
#include "engine/math/scene_vec.h"
#include "scene/scene_animation.h"
#include "effects/particles/scene_particle_burst_api.h"

/*
 * Scene objects: pushable/breakable blocks and similar props (types 0-6) that live in a fixed
 * pool owned by a task (struct SceneObjectManager). Each object is 0x1A8 bytes and embeds a
 * body (position/velocity), an animation, a draw record, two colliders and a terrain-cell
 * reference. Objects move between cells of the terrain map one step at a time, fall when the
 * block under them is removed, flash when hit and disappear when their health is gone.
 */

/* Three 16-bit extents/offsets passed by pointer to the particle and collider helpers. */
struct SceneExtent {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

/* One terrain cell (4 bytes). */
/* One terrain cell: 256 world units square; a level is 256 units of height (a
 * level-1 floor is y = 256). In the HP data: kind 1 marks stairs/slopes; flags
 * 0x0003 outside the stage, 0x040B walls, 0x000A door cells (docs/ASSETS.md). */
struct SceneCell {
    u8 height; /* low nibble: height level (0-15); high nibble: kind (0 = solid floor, else open) */
    u8 tile;
    u16 flags; /* bit 0 occupied by an object, bit 3 blocks entry, 0x20 (guess) */
};

static inline u16 SceneCell_GetFlags(const struct SceneCell *cell)
{
    return cell->flags;
}

/* Position and velocity block, shared with the generic entity code (SceneBody_Init etc.). */
struct SceneBody {
    u32 id; /* script id (the 'n' parameter) */
    u32 flags;
    u32 unknown;
    struct SceneVec position;
    s16 vx, vy, vz;
    u8 tail[46];
};



struct SceneDraw {
    u32 flags; /* bit 0: hidden (owner is not the current one) */
    u8 pad4[12];
    s16 mode; /* sprite variant / flash step */
    u8 tail[10];
};

struct SceneCollider {
    u8 pad0[6];
    u16 state; /* bit 2: collision disabled */
    u8 pad8[44];
    u32 contactFlags, contactFlags2; /* what the colliding thing is (attack kind bits) */
    u8 tail[24];
};

static inline u32 SceneCollider_ReadContactFlags(const struct SceneCollider *contact, u32 mask)
{
    return contact->contactFlags & mask;
}

static inline u32 SceneCollider_ReadReactionFlags(const struct SceneCollider *contact, u32 mask)
{
    return contact->contactFlags2 & mask;
}

struct SceneLink {
    u16 id;
    u8 pad2[2];
    struct SceneVec *position;
    u8 tail[40];
};

/* Reference to the terrain cell the object occupies, filled in by TerrainModifiers_RegisterCell. */
struct SceneCellRef {
    u16 unknown0;
    u16 index;             /* index into the terrain cell array */
    struct SceneCell cell; /* working copy of the cell (height of the object's top) */
    u8 pad8[8];
};

struct SceneObject {
    u8 type;  /* 0-6: 1 = movable block, 5 = limited-push block, 6 = palette switch */
    u8 flags; /* 2 no body, 4 flash on hit, 8 stays solid, 0x10 owned, 0x20 (guess) */
    u16 timer;
    u16 kind; /* 0 idle, 1 stepping, 2 falling, 3 disappearing */
    u16 unknown6;
    u32 owner;
    s16 dx, dz;   /* step velocity */
    u8 direction; /* 0 north, 1 east, 2 south, 3 west */
    u8 entered;   /* set to 1 when a new update state starts; the state clears it on first run */
    u16 targetY;
    u16 duration; /* frames (or speed) of the current step */
    s16 health;
    u8 damageTimer; /* counts down while the object is invulnerable */
    u8 mode;        /* 0 normal, 2 removed, 3 destroyed by script */
    u16 basePalette;
    u16 animation;
    u8 active; /* slot in use */
    u8 animationTimer;
    u8 phase;
    u8 cooldown;
    u8 randomPhase;
    u8 unk23;
    u8 pushLimited; /* type 5: pushes are limited */
    u8 pushesLeft;
    u8 pad26[2];
    u32 script; /* script handle run once when the object is removed */
    struct SceneBody body;
    struct SceneAnimation animationData;
    struct SceneDraw draw;
    struct SceneVec center;
    u8 padB4[8];
    struct SceneCellRef cellRef;
    struct SceneCollider collider, stepCollider;
    struct SceneLink link;
    void (*update)(struct SceneObject *);
};

struct SceneObjectManager {
    struct Task task;
    u16 count; /* capacity of objects[] */
    u8 frame, descending;
    s16 fade;
    u8 fading, stopFade;
    u32 fromA, fromB, toA, toB;
    u32 paletteA[8], paletteB[8];
    struct SceneObject *objects;
};

/* 0x02000024 */
extern struct SceneObjectManager *gSceneObjectManager;

/* Engine helpers shared by the functions below. */
void ImpactBurst_EmitDirectional(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, void *);

/* Update states (stored in SceneObject.update). */
void PuzzleBlock_UpdateObjectStep(struct SceneObject *);
void PuzzleBlock_UpdateFallingObject(struct SceneObject *);
void PuzzleBlock_UpdateObjectDisappearance(struct SceneObject *);
void PuzzleBlock_UpdateObjectFlash(struct SceneObject *);
void PuzzleBlock_UpdateObjectIdle(struct SceneObject *); /* idle state */
void PuzzleBlock_ApplyTerrainInteraction(struct SceneCollider *, void *, struct SceneObject *);
void PuzzleBlock_NoOpUpdate(void);

void PuzzleBlock_SpawnEdgeParticles(struct SceneVec *position, s32 offset);
u8 PuzzleBlock_CanEnterNeighborCell(struct SceneObject *, s32 index, s32 direction);
void PuzzleBlock_PlayNeighborStepSound(u32 type);
u8 PuzzleBlock_CheckNeighborHeight(struct SceneObject *, s32 index, s32 direction);
u8 PuzzleBlock_IsObjectSurrounded(struct SceneObject *);
void PuzzleBlock_PlayMovementEffect(struct SceneObject *);
void PuzzleBlock_PlayNeighborMovementEffect(struct SceneObject *);
void PuzzleBlock_PlayPushSound(s32 type);
void PuzzleBlock_DropObjectsAbove(struct SceneObject *);
u32 PuzzleBlock_DestroyObject(struct SceneObject *);
s32 PuzzleBlock_DestroyAllObjects(void);
s32 PuzzleBlock_ActivateTypeOneObjects(void);
s32 PuzzleBlock_StopSelectedObject(void);
s32 PuzzleBlock_DestroyObjectsByScriptId(void);
void PuzzleBlock_PlayObjectBreakEffect(struct SceneObject *);
void PuzzleBlock_PlayObjectSplashEffect(struct SceneObject *);
void PuzzleBlock_PlayObjectImpactEffect(struct SceneObject *);
void PuzzleBlock_SpawnDirectionalDust(struct SceneObject *);
s32 PuzzleBlock_TryStepDown(struct SceneObject *, s32 index, s32 direction);
s32 PuzzleBlock_TryPushNeighbor(struct SceneVec *position, s32 direction);
s32 PuzzleBlock_TryPushNeighborByIndex(struct SceneVec *position, u32 index);
s32 PuzzleBlock_Update(struct SceneObjectManager *);
u32 PuzzleBlock_DestroyTask(struct SceneObjectManager *);
s32 PuzzleBlock_InitPool(struct SceneObjectManager *, void *);
struct SceneObjectManager *PuzzleBlock_CreateTask(void *);
struct SceneObject *PuzzleBlock_FindFreeSlot(struct SceneObjectManager *);
struct SceneObject *PuzzleBlock_FindByScriptId(struct SceneObjectManager *, u32 id);
s32 PuzzleBlock_CreateObject(u32 id, s32 type, u32 flags, struct SceneVec *position,
                              u32 animationResource, s32 drawMode, s32 health, s32 basePalette,
                              s32 animation, u32 script);
s32 PuzzleBlock_CreateFromScript(void);
s32 PuzzleBlock_SwitchPaletteFromScript(void);

#endif
