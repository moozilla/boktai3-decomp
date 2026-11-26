#ifndef GUARD_NAMED_ACTOR_COMMANDS_H
#define GUARD_NAMED_ACTOR_COMMANDS_H
#include "objects/moving_platforms/named_object.h"
#include "player/player.h"
#include "effects/shadows/terrain_shadow.h"
#include "scene/motion/scene_actor_terrain.h"
struct NamedObject *MovingPlatform_FindByName(s32);
struct NamedObject *MovingPlatform_GetFollowTarget(void);
u32 TetherActor_TestStatusFlags(u32);
void MovingPlatform_ReleaseMember(void *);
void SceneBody_Unregister(void *);
void Route_Start(void *, u32, u32, u32);
void Player_ClearAttachmentFlags(void *, s32);
u32 Player_TestAttachmentFlags(void *, u32);
#endif
