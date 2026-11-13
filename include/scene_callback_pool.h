#ifndef FOUNDRY_SCENE_CALLBACK_POOL_H
#define FOUNDRY_SCENE_CALLBACK_POOL_H

#include "contact_swarm.h"
#include "scene_animation.h"
#include "engine.h"

/* Spatial collider used by the native projectile family (0x40 bytes). */
struct NativeEffectCollider {
    u32 flags, mask, category;
    u16 kind;
    u8 unknownE, registrationFlags;
    s16 x, y, z, unknown16;
    s16 extentX, extentY, extentZ, unknown1E;
    s16 offsetX, offsetY, offsetZ;
    u8 unknown26[11];
    s8 bucket;
    u8 unknown32[4], parameter, unknown37;
    void *owner;
    void (*callback)(void);
};
struct NativeEffectVector {
    s32 x, y, z, w;
};
struct NativeEffectSlotKey {
    u16 id;
    u8 entered, active;
};
struct ScenePoolSlot {
    void (*callback)(struct ScenePoolSlot *, u32);
    struct NativeEffectCollider collider;
    struct SceneAnimation graphics;
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState animation;
    void *data;
    struct NativeEffectVector position, velocity;
    s32 height, speed;
    u16 lifetime;
    u8 angle, state;
    struct NativeEffectSlotKey key;
    s32 direction;
    u32 timer;
};
struct SceneCallbackPool {
    struct Task task;
    struct ScenePoolSlot slots[16];
    u32 frame;
};
typedef char SceneCallbackPoolSizeCheck[(sizeof(struct SceneCallbackPool) == 0xD9C) ? 1 : -1];

#endif
