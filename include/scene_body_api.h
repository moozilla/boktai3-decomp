#ifndef GUARD_SCENE_BODY_API_H
#define GUARD_SCENE_BODY_API_H

#include "global.h"

struct SceneActorBody;
struct ContactPosition;

u32 SceneBody_Init(struct SceneActorBody *body, u32 id, const struct ContactPosition *position,
                   u8 direction, u8 mode, void *owner);
u32 SceneBody_SetSprite(struct SceneActorBody *body, void *sprite);
u32 SceneBody_SetMovementProbe(struct SceneActorBody *body, void *probe, u16 extentX, u16 extentZ);

#endif
