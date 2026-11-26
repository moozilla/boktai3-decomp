#include "engine/video/anim_sprite.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
void sub_08220E6C(struct AnimSprite *anim, struct AnimSpriteResource *data, u16 sequence, u16 index)
{
    struct AnimationKeyframeSequence *table = data->sequences;
    struct AnimationKeyframeSequence *entry = (struct AnimationKeyframeSequence *)((sequence * 8) + (u32)table);
    struct AnimationKeyframe *frames = (struct AnimationKeyframe *)(data->frameData + entry->offset);
    struct AnimationKeyframe *frame;
    anim->frameIndex = index;
    if (index >= entry->frameCount)
        anim->frameIndex = entry->frameCount - 1;
    frame = &frames[anim->frameIndex];
    Anim_LoadKeyframe(anim, data, frame->key, anim->mode);
    anim->frameCount = entry->frameCount;
    anim->duration = Scale6(frame->duration * anim->durationScale);
    if (anim->duration == 0)
        anim->duration = 1;
    anim->elapsed = 0;
    anim->value6 = 0;
    anim->status = 0;
    if (anim->elapsed + 1 >= anim->duration) {
        if (anim->playbackFlags & 2) {
            if (anim->frameIndex != 0)
                return;
        } else if (anim->frameIndex + 1 < anim->frameCount)
            return;
        anim->status = 1;
    }
}
