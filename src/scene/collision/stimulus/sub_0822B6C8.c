#include "scene/collision/contact_position.h"
#include "global.h"
#include "scene/collision/scene_stimulus.h"
u8 sub_0822B5FC(struct ContactPosition *);
/* Posts a stimulus at (x, z) into the first free slot (id < 0), unless the
   point lies in a suppression box (sub_0822B5FC). Flag 4 marks it fresh. */
void sub_0822B6C8(u16 x, u16 z, u16 id, u16 range, u8 flags, u16 tag)
{
    struct SceneStimulusState *ctx = gSceneStimulusState;
    struct ContactPosition pos;
    struct SceneStimulus *s;
    s32 i;
    s16 zero;
    if (ctx != NULL) {
        pos.x = x;
        pos.y = 0;
        pos.z = z;
        if (!sub_0822B5FC(&pos)) {
            i = 0;
            zero = 0;
            flags |= 4;
            for (; i < 8; i++) {
                s = &ctx->stimuli[i];
                if ((s16)s->id < 0) {
                    s->x = x;
                    s->y = zero;
                    s->z = z;
                    s->tag = tag;
                    s->id = id;
                    s->range = range;
                    s->flags = flags;
                    break;
                }
            }
        }
    }
}
