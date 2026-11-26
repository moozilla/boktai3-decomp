#include "scene/actor/scene_actor_record.h"
static inline u8 Enabled(void)
{
    u32 mask = 4;
    struct GameState *base = gGameState;
    if (base->flags868 & mask)
        return TRUE;
    return FALSE;
}
void SceneActorRecord_Tick(void)
{
    if (Enabled()) {
        s32 i;
        for (i = 0; i < 20; i++) {
            struct SceneActorRecord *entry = &SCENE_ACTOR_RECORDS[i];
            if (entry->id && !--entry->ticks)
                entry->id = 0;
        }
    }
}
