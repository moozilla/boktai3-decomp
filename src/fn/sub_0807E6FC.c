// MATCHING_CONCESSION: register-pinning
#include "scene_actor_effects.h"
void SceneActorEffects_EmitRandomOffsets(struct SceneActorRegistryActor *actor, s32 count, u32 mode,
                                         s32 inputSpan, s32 spanY, u32 value)
{
    struct SceneActorEffectsOffset offset;
    s32 spanX;
    struct SceneActorRegistryActor *owner = actor;
    u32 kind = mode;
    s32 incomingSpan = inputSpan;
    spanX = incomingSpan;
    offset.z = 0;
    offset.padding = 0;
    if (count > 0) {
        register u32 *index asm("r6") = &gRandomIndex;
        u32 mask = 0x3FF;
        register s32 left asm("r5") = count;
        do {
            register const u16 *table asm("r4") = gUnk_0203B400;
            register s32 random asm("r0");
            *index = (*index + 1) & mask;
            random = table[*index];
            random = Mod(random >> 4, spanX);
            asm("" : "+r"(spanX));
            offset.x = random - (spanX >> 1);
            {
                u32 next = *index + 1;
                u32 maskCopy = mask;
                *index = next & maskCopy;
            }
            random = table[*index];
            offset.y = (s32)(0u - (u32)Mod(random >> 3, spanY) - 6u);
            EffectGroup_Spawn((u8 *)owner + 0x47C, kind, value, &offset);
        } while (--left != 0);
    }
}
