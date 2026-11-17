#include "scene_input_widget.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
void sub_08220E6C(struct SceneInputWidget *anim, struct SceneInputResource *data, u16 sequence, u16 index)
{
    struct AnimationKeyframeSequence *table = data->sequences;
    struct AnimationKeyframeSequence *entry = (struct AnimationKeyframeSequence *)((sequence * 8) + (u32)table);
    struct AnimationKeyframe *frames = (struct AnimationKeyframe *)(data->frameData + entry->offset);
    struct AnimationKeyframe *frame;
    anim->frameTimer = index;
    if (index >= entry->frameCount)
        anim->frameTimer = entry->frameCount - 1;
    frame = &frames[anim->frameTimer];
    Anim_LoadKeyframe(anim, data, frame->key, anim->attribute1B);
    anim->frameDuration = entry->frameCount;
    anim->attributeE = Scale6(frame->duration * anim->scale);
    if (anim->attributeE == 0)
        anim->attributeE = 1;
    anim->fieldC = 0;
    anim->value6 = 0;
    anim->status = 0;
    if (anim->fieldC + 1 >= anim->attributeE) {
        if (anim->value18 & 2) {
            if (anim->frameTimer != 0)
                return;
        } else if (anim->frameTimer + 1 < anim->frameDuration)
            return;
        anim->status = 1;
    }
}
