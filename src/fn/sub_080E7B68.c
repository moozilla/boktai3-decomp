#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_motion.h"
#include "engine.h"
#include "display_sequence_inline.h"
static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline u32 Cell(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
// confidence=low: resets Mimic pursuit state and advances its idle drawable.
void Mimic_ResetPursuitDrawable(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    *(u16 *)(context + 0x65A) = 0;
    *(u16 *)(context + 0x65C) = 0;
    if (Test(gEngineFlags, 0x4800)) {
        *(struct ContactPosition *)(context + 0x650) = actor->body.position;
        *(u16 *)(context + 0x658) = Cell(&actor->body.position);
    }
    SceneActorAnimation_SelectEntryFlags((void *)actor, 0,
                                         SceneActor_GetDirectionFlags(actor->body.direction));
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
