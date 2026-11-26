#ifndef GUARD_SCENE_INDICATOR_H
#define GUARD_SCENE_INDICATOR_H

#include "scene/collision/contact_collider.h"
#include "engine/engine.h"
#include "player/player.h"
#include "engine/video/sprite.h"

struct Vec {
    s16 x, y, z, pad;
};

/* One blinking terrain marker (0xB4 bytes). */
struct SceneIndicator {
    u8 pad0[8];
    u32 timer; /* frames in the current phase (mode 0) */
    int id;    /* script id ('n') */
    u8 active;
    s8 frame;                                            /* base sprite frame */
    u8 animation, index, count, visible, mode, disabled; /* mode: 0 timed, 1 triggered, 2 manual */
    int delay, period, value;
    u16 times[8];  /* phase durations, alternating hidden/visible */
    struct ContactCollider collider;
    struct DisplaySequenceSprite sprite;
};

static inline void IndicatorSprite_SetFrame(struct DisplaySequenceSprite *s, int frame)
{
    s->frame = frame;
}

/* Scene indicator controller task (0xBD0 bytes, priority list 8). */
struct SceneIndicators {
    struct Task task;
    int count;
    struct SpriteRenderResource animations[4];
    struct SceneIndicator indicators[16];
    int tile; /* terrain cell index under the player */
};


/* 0x0200004C */
extern struct SceneIndicators *gSceneIndicators;

struct SceneIndicators *SceneIndicators_GetInstance(void);
void SceneIndicator_Initialize(struct SceneIndicators *m, struct SceneIndicator *p);
void SceneIndicator_Update(struct SceneIndicators *owner, struct SceneIndicator *p);
void SceneIndicator_Destroy(struct SceneIndicators *owner, struct SceneIndicator *p);
int SceneIndicators_Update(struct SceneIndicators *m);
u32 SceneIndicators_Destroy(struct SceneIndicators *m);
int SceneIndicators_Initialize(struct SceneIndicators *m);
struct SceneIndicators *SceneIndicators_Create(void);
struct SceneIndicator *SceneIndicators_FindFreeSlot(struct SceneIndicators *m);
struct SceneIndicator *SceneIndicators_FindById(struct SceneIndicators *m, u32 key);
int SceneIndicator_CreateFromScript(void);
int SceneIndicator_Pause(void);
int SceneIndicator_Resume(void);

u8 *TerrainModifiers_FindCellOverride(int, int);
int Terrain_GetHigherNeighborMask(void *);
void Sprite_ApplyAnimation(void *, void *);
void Collider_SetAttackPowerAndForce(void *, int, int);
void Collider_SetAttackProperties(void *, int, int, int, int, int);

#endif
