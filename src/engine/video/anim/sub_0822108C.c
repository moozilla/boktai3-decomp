#include "global.h"
#include "engine/video/anim_sprite.h"

s32 sub_0822108C(u32 unused, struct AnimSpriteResource *resource, s32 sequence)
{
    struct AnimationKeyframeSequence *table = resource->sequences;
    struct AnimationKeyframeSequence *entry =
        (struct AnimationKeyframeSequence *)((sequence * 8) + (u32)table);
    struct AnimationKeyframe *frames =
        (struct AnimationKeyframe *)(resource->frameData + entry->offset);
    s32 i, sum;

    for (i = 0, sum = 0; i < entry->frameCount; i++)
        sum += frames[i].duration;
    return sum;
}
