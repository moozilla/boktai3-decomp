#include "global.h"
#include "scene_stimulus.h"
u8 SceneStimulus_TestSquareOverlap(s32, s32, s32, s32, s32, s32);
s8 sub_0822B7E4(s32 a, s32 b, s32 c, u8 mask, u16 value, s32 start)
{
    struct SceneStimulusState *ctx = gSceneStimulusState;
    struct SceneStimulus *slot;
    s32 index;
    if (ctx != NULL)
        goto begin;
    goto fail;
found:
    return index;
begin:
    index = start;
    if (index <= 7) {
        slot = (struct SceneStimulus *)(index * 16 + (u32)ctx->stimuli);
        do {
            if (!(slot->flags & 4) && (!slot->tag || slot->tag != value)) {
                if (slot->flags & 0x80)
                    goto found;
                if ((s16)slot->id >= 0 && (slot->flags & mask) &&
                    SceneStimulus_TestSquareOverlap(slot->x, slot->z, (s16)slot->id, a, b, c))
                    goto found;
            }
            slot++;
            index++;
        } while (index <= 7);
    }
fail:
    return -1;
}
