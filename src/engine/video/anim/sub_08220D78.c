#include "engine/video/anim_sprite.h"

s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);

static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}

void Anim_SetKeyframes(struct AnimSprite *anim, struct AnimSpriteResource *data, u16 sequence, u16 mode,
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
            off = anim->frameIndex * 4;
        frame = (struct AnimationKeyframe *)((u8 *)frames + off);
    }
have_frame:
    if (anim->sequenceId != sequence || (flags & 4))
        Anim_LoadKeyframe(anim, data, frame->key, (u8)mode);
    anim->sequenceId = sequence;
    anim->frameCount = seqs[sequence].frameCount;
    anim->duration = Scale6(frame->duration * anim->durationScale);
    if (anim->duration == 0)
        anim->duration = 1;
    anim->playbackFlags = flags;
    anim->frames = frames;
    anim->mode = mode;
    if (flags & 1)
        return;
    if (flags & 2)
        anim->frameIndex = anim->frameCount - 1;
    else
        anim->frameIndex = 0;
    anim->value6 = 0;
    anim->elapsed = 0;
    anim->status = 0;
    anim->durationScale = 0x40;
}
