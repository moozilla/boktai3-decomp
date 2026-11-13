#ifndef GUARD_SCENE_PROJECTILE_PARAMS_H
#define GUARD_SCENE_PROJECTILE_PARAMS_H
#include "contact_position.h"
/* Parameters copied by the scene-projectile pool; setter interpretations tentative. */
struct SceneProjectileParams {
    u16 movement, lifetime, trackingFrames;
    u8 direction, turnSpeed;
    s32 speed;
    u32 turnParameter;
    void *callback;
    u32 collisionProperties[5];
    struct ContactPosition bounds, offset;
    const struct ContactPosition *target;
    u32 graphicsId, frame;
    s32 palette;
    u32 script;
    u16 sequence, mode, unknown50, unknown52, trackingEnabled, unknown56;
};
#endif
