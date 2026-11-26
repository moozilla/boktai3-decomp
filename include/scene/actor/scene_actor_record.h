#ifndef GUARD_SCENE_ACTOR_RECORD_H
#define GUARD_SCENE_ACTOR_RECORD_H
#include "engine/engine.h"
#include "system/game_state/game_state.h"
#include "scene/actor/scene_actor_registry.h"
#include "scene/collision/scene_actor_damage_visibility.h"
/* Twenty six-byte records; zero id marks a free slot. */
struct SceneActorRecord {
    u16 key, id, ticks;
} __attribute__((packed, aligned(2)));
#define SCENE_ACTOR_RECORDS ((struct SceneActorRecord *)((u8 *)gGameState + 0x670))
struct SceneActorRecord *SceneActorRecord_FindPair(u32, u32);
struct SceneActorRecord *SceneActorRecord_FindEmpty(void);
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRecord_RecordPair(u32, u32);

/* Record local defeats unless the actor suppresses the scene record. */
static inline void SceneActorRecord_RecordDefeat(struct ChaseActor *actor)
{
    if (!SceneActorDamage_TestFlags(gEngineFlags, 0x4800)) {
        u32 suppressed = 0x800;
        if (!(actor->base.statusFlags & suppressed))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->base.body.id);
    }
}
#endif
