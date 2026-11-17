#include "scene_input_widget.h"

s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);

static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}

void Anim_SetKeyframes(struct SceneInputWidget *anim, struct SceneInputResource *data, u16 sequence, u16 mode,
                       u16 flags)
{
    struct AnimationKeyframeSequence *seqs;
    struct AnimationKeyframe *frames;
    struct AnimationKeyframe *frame;

    if (sequence >= data->sequenceCount)
        sequence = 0;
    seqs = data->sequences;
    frames = (struct AnimationKeyframe *)(data->frameData + seqs[sequence].offset);
    {
        s32 off;
        if (!(flags & 1)) {
            if (flags & 2)
                off = seqs[sequence].frameCount * 4 - 4;
            else {
                frame = frames;
                goto have_frame;
            }
        } else
            off = anim->frameTimer * 4;
        frame = (struct AnimationKeyframe *)((u8 *)frames + off);
    }
have_frame:
    if (anim->frame != sequence || (flags & 4))
        Anim_LoadKeyframe(anim, data, frame->key, (u8)mode);
    anim->frame = sequence;
    anim->frameDuration = seqs[sequence].frameCount;
    anim->attributeE = Scale6(frame->duration * anim->scale);
    if (anim->attributeE == 0)
        anim->attributeE = 1;
    anim->value18 = flags;
    anim->frames = frames;
    anim->attribute1B = mode;
    if (flags & 1)
        return;
    if (flags & 2)
        anim->frameTimer = anim->frameDuration - 1;
    else
        anim->frameTimer = 0;
    anim->value6 = 0;
    anim->fieldC = 0;
    anim->status = 0;
    anim->scale = 0x40;
}
