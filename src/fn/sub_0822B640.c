#include "global.h"
#include "scene_stimulus.h"
/* Per-frame stimulus aging: a slot posted this frame (flag 4) loses the flag;
   one that was not re-posted is cleared (id 0xFFFF, flags 0). */
void sub_0822B640(void)
{
    struct SceneStimulusState *ctx = gSceneStimulusState;
    s32 i;
    if (ctx != NULL) {
        for (i = 0; i < 8; i++) {
            u16 f = ctx->stimuli[i].flags & 4;
            if (f)
                ctx->stimuli[i].flags &= ~4;
            else {
                ctx->stimuli[i].id |= 0xFFFF;
                ctx->stimuli[i].flags = 0;
            }
        }
    }
}
