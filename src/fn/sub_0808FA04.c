#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
s32 Mod(s32, s32);
extern const u16 gUnk_08605D58[];
// confidence=low: emits the profile's six-frame movement pulse at animation frame boundaries.
s32 Slime_GetAnimationMovePulse(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    s32 result;
    if (sequence->elapsed)
        result = 0;
    else {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
        result = Mod(sequence->index, 6);
        {
            const u16 *table = gUnk_08605D58;
            result = table[result];
        }
    }
    return result;
}
