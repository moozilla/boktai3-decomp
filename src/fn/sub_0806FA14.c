#include "scene_actor_record.h"
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
    int location;
    if (Enabled()) {
        u32 offset;
        u32 tableOffset;
        s32 i;
        location = 0x02000710;
        offset = 0;
        tableOffset = 0x670;
        i = 19;
        do {
            struct SceneActorRecord *entry =
                (void *)((u8 *)*(struct GameState **)location + (offset + tableOffset));
            if (entry->id) {
                entry->ticks--;
                if (!entry->ticks)
                    entry->id = 0;
            }
            offset += 6;
        } while (--i >= 0);
    }
}
