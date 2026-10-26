#ifndef GUARD_SCENE_ACTOR_RECORD_H
#define GUARD_SCENE_ACTOR_RECORD_H
#include "engine.h"
#include "game_state.h"
#include "scene_actor_registry.h"
/* Twenty six-byte records; zero id marks a free slot. */
struct SceneActorRecord {
    u16 key, id, ticks;
} __attribute__((packed, aligned(2)));
#define SCENE_ACTOR_RECORDS ((struct SceneActorRecord *)((u8 *)gGameState + 0x670))
struct SceneActorRecord *SceneActorRecord_FindPair(u32, u32);
struct SceneActorRecord *SceneActorRecord_FindEmpty(void);
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRecord_RecordPair(u32, u32);
#endif
