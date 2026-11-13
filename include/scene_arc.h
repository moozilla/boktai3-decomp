#ifndef GUARD_SCENE_ARC_H
#define GUARD_SCENE_ARC_H
#include "scene_pool.h"
#include "scene_vec.h"
#include "scene_object.h"
#include "contact_swarm_sprite.h"
#include "display_sequence.h"
#include "sprite_effect_controller.h"

/* Eight 0x130-byte arc slots (0803EE58-0803F650), followed by their
 * projected ground-effect companions. Graphical shadow ownership is tentative. */
struct SceneGroundEffect {
    u8 active, kind, counter, unknown3;
    u16 value4, value6;
    const struct ScenePoolVec32 *position;
    u32 flags;
    u32 unknown10;
    u16 tile;
    u8 width, height;
    u8 unknown18[3];
    u8 mode, priority;
    u8 unknown1d[7];
    struct SceneVec screen;
    u8 unknown2c[8];
    void (*update)(struct SceneGroundEffect *);
    struct SceneGroundEffect *prev, *next;
};
struct SceneArcSlot {
    u32 frame, flightDuration, endFrame, releaseFrame;
    s32 height;
    u32 grounded;
    struct ScenePoolVec32 start, position, target;
    u8 unknown48[0x10];
    u8 collider[0x40];
    struct SceneAnimation animationData;
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState animation;
    struct SceneGroundEffect ground;
};
struct SceneArcManager {
    struct Task task;
    u32 activeMask;
    void *animationResource;
    struct SceneArcSlot slots[8];
};
struct SceneGroundManager {
    struct Task task;
    const struct SpriteEffectTileResource *resource;
    struct SceneGroundEffect *head;
};
struct SceneArcCamera {
    u8 unknown0[0x44];
    s32 x;
};
struct SceneArcPackedVector {
    s32 x : 16, y : 16, z : 16;
};
void SceneArc_HandleCollision(void);
void SceneArc_InitializeSlot(struct SceneArcManager *, struct SceneArcSlot *, s32);
void SceneArc_DestroySlot(struct SceneArcManager *, struct SceneArcSlot *, s32);
void SceneArc_ReleaseSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);
void SceneArc_UpdateSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);
u32 SceneArc_UpdateManager(struct SceneArcManager *);
u32 SceneArc_DestroyManager(struct SceneArcManager *);
s32 SceneArc_InitializeManager(struct SceneArcManager *);
struct SceneArcManager *SceneArc_CreateManager(void);
struct SceneArcSlot *SceneArc_FindFreeSlot(struct SceneArcManager *, s32 *);
s32 SceneArc_Create(const struct ScenePoolVec32 *, const struct ScenePoolVec32 *, u32, u32, s32);
s32 GroundEffect_Link(struct SceneGroundManager *, struct SceneGroundEffect *);
s32 GroundEffect_Unlink(struct SceneGroundManager *, struct SceneGroundEffect *);
void GroundEffect_EnableProjection(struct SceneGroundEffect *);
void GroundEffect_DisableProjection(struct SceneGroundEffect *);
void GroundEffect_IgnoreUpdate(struct SceneGroundEffect *);
void GroundEffect_UpdateProjection(struct SceneGroundEffect *);
u32 GroundEffect_UpdateManager(struct SceneGroundManager *);
u32 GroundEffect_DestroyManager(void);
s32 GroundEffect_InitializeManager(struct SceneGroundManager *, void *);
struct SceneGroundManager *GroundEffect_CreateManager(void *, u32);
s32 GroundEffect_Initialize(struct SceneGroundEffect *, const struct ScenePoolVec32 *, u32);
u32 GroundEffect_Destroy(struct SceneGroundEffect *);
extern struct SceneArcCamera *gSceneCamera;
#endif
