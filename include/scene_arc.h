#ifndef GUARD_SCENE_ARC_H
#define GUARD_SCENE_ARC_H
#include "scene_pool.h"
#include "scene_vec.h"

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
struct SceneArcSprite {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
    u8 unknown12[10];
    struct SceneVec screen;
    u8 unknown24[8];
};
struct SceneArcAnimation {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unknownA, scale, timer;
};
struct SceneArcSlot {
    u32 frame, flightDuration, endFrame, releaseFrame;
    s32 height;
    u32 grounded;
    struct ScenePoolVec32 start, position, target;
    u8 unknown48[0x10];
    u8 collider[0x40];
    u8 animationData[0x1c];
    struct SceneArcSprite sprite;
    struct SceneArcAnimation animation;
    struct SceneGroundEffect ground;
};
struct SceneArcManager {
    u8 task[0x18];
    u32 activeMask;
    void *animationResource;
    struct SceneArcSlot slots[8];
};
struct SceneGroundManager {
    u8 task[0x18];
    const u8 *resource;
    struct SceneGroundEffect *head;
};
struct SceneArcCamera {
    u8 unknown0[0x44];
    s32 x;
};
extern struct SceneArcCamera *gSceneCamera;
#endif
