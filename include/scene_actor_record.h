#ifndef GUARD_SCENE_ACTOR_RECORD_H
#define GUARD_SCENE_ACTOR_RECORD_H
#include "engine.h"
#include "game_state.h"
#include "scene_actor_registry.h"
/* Twenty six-byte records; zero id marks a free slot. */
struct SceneActorRecord {u16 key,id,ticks;} __attribute__((packed,aligned(2)));
#define SCENE_ACTOR_RECORDS ((struct SceneActorRecord*)((u8*)gUnk_02000710+0x670))
struct SceneActorRecord *sub_0806F9D8(u32,u32);
struct SceneActorRecord *sub_0806F9AC(void);
struct SceneActorRegistryManager *sub_08072B44(void);
void sub_0806FA74(u32,u32);
#endif
