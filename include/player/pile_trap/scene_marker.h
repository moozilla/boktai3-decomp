#ifndef GUARD_SCENE_MARKER_H
#define GUARD_SCENE_MARKER_H

#include "objects/scene_object.h"
#include "scene/scene_animation.h"
#include "scene/collision/contact_collider.h"

/* A placed scene marker: a body, an attack collider and two sprites (the player's barrier,
 * PileTrap_*, is one; created by Task_Create(8, 0x150) and SceneMarker_InitSprites). */
struct SceneMarkerSprite {
    u32 flags; /* bit 0 hides the sprite */
    u8 unknown4[3];
    u8 mode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct SceneVec position;
    u8 unknown24[8];
};

static inline void SceneMarkerSprite_SetFrame(struct SceneMarkerSprite *sprite, u16 frame)
{
    sprite->frame = frame;
}

struct SceneMarker {
    u8 task[0x18];
    struct SceneBody body;
    u32 source;
    struct ContactCollider collider;
    s32 colliderState; /* 0xB8: 0 hidden, 1 placed, 2 recovering, 3 active (tentative) */
    struct SceneAnimation secondaryData, primaryData;
    struct SceneMarkerSprite primary;
    struct SceneMarkerSprite secondary;
    u32 state; /* 0x14C: frame counter of the current state */
};

typedef char SceneMarkerSizeCheck[sizeof(struct SceneMarker) == 0x150 ? 1 : -1];

#endif
