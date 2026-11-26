#ifndef GUARD_NAMED_ACTOR_COMMANDS_H
#define GUARD_NAMED_ACTOR_COMMANDS_H
#include "objects/named_object/named_object.h"
#include "player/player.h"
#include "effects/shadows/terrain_shadow.h"
#include "scene/actor/scene_actor_terrain.h"
struct NamedObject *NamedObject_FindByName(s32);
struct NamedObject *NamedObject_GetFollowTarget(void);
u32 TetherActor_TestStatusFlags(u32);
void ActorGroup_ReleaseMember(void *);
void SceneBody_Unregister(void *);
void Route_Start(void *, u32, u32, u32);
void Player_ClearAttachmentFlags(void *, s32);
u32 Player_TestAttachmentFlags(void *, u32);
#endif
