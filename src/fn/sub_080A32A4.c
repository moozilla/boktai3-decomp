#include "scene_actor_reaction.h"
#include "engine.h"
/* SceneActorControl_UpdateEntryWait: tentative */

static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
        return 1;
    }
    return 0;
}

#ifdef NONMATCHING
// fuzzy: 99.2%
void SceneActorControl_UpdateEntryWait(struct SceneActorRegistryResetView *s)
{
    if (TakeEntry(s)) {
        void *fn = s->entryTarget224;
        u32 two = 2;
        u8 *ready = &s->field2B2;
        u32 zero = 0;
        *ready = 1;
        s->field2B0 = zero;
        s->field2B5 = two;
        s->target = fn;
        s->field2C8 = zero;
        s->flags114 &= ~1;
        s->word104 = zero;
    } else {
        u8 result = Callback_InvokeActor(s, s->update270);
        if (result == 0) {
            if (s->field2B0) {
                u32 seven = 7;
                s->state2A8 = result;
                s->phase2A9 = result;
                s->behavior2AA = seven;
                s->counter2AC = result;
                s->field2B1 = 1;
                SceneActorReaction_SetControl(s, 1, s->entryTarget228);
            } else {
                s->counter2AC++;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorControl_UpdateEntryWait);
#endif
