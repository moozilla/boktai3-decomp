#include "scene_actor_effects.h"
void SceneActorEffects_EmitRandomOffsets(struct SceneActorRegistryActor *actor, s32 count, u32 mode,
                                         s32 spanX, s32 spanY, u32 value)
{
    struct SceneActorEffectsOffset offset;
    s32 i;
    offset.z = 0;
    offset.padding = 0;
    for (i = 0; i < count; i++) {
        s32 random;
        union RandomTableView table = {gRandomTable};
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        random = table.samples[gRandomIndex];
        offset.x = Mod(random >> 4, spanX) - (spanX >> 1);
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        random = table.samples[gRandomIndex];
        offset.y = -Mod(random >> 3, spanY) - 6;
        EffectGroup_Spawn((u8 *)actor + 0x47C, mode, value, &offset);
    }
}
