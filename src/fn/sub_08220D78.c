#include "global.h"

struct AnimFrame {
    u16 key, duration;
};
struct AnimSequence {
    u8 pad0[2];
    u16 frameCount;
    u32 offset;
};
struct AnimData {
    u8 pad0[4];
    u16 sequenceCount;
    u8 pad6[0x10 - 6];
    struct AnimSequence *sequences;
    u8 pad14[4];
    u8 *frameData;
};
struct AnimState {
    u8 pad0[6];
    u16 unk6;
    u8 pad8[4];
    u16 timer, duration, scale, sequence, frame, frameCount, flags;
    u8 pad1a;
    u8 mode;
    u8 pad1c;
    u8 status;
    u8 pad1e[0x54 - 0x1e];
    struct AnimFrame *frames;
};
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);

static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}

void Anim_SetKeyframes(struct AnimState *anim, struct AnimData *data, u16 sequence, u16 mode,
                       u16 flags)
{
    struct AnimSequence *seqs;
    struct AnimFrame *frames;
    struct AnimFrame *frame;

    if (sequence >= data->sequenceCount)
        sequence = 0;
    seqs = data->sequences;
    frames = (struct AnimFrame *)(data->frameData + seqs[sequence].offset);
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
            off = anim->frame * 4;
        frame = (struct AnimFrame *)((u8 *)frames + off);
    }
have_frame:
    if (anim->sequence != sequence || (flags & 4))
        Anim_LoadKeyframe(anim, data, frame->key, (u8)mode);
    anim->sequence = sequence;
    anim->frameCount = seqs[sequence].frameCount;
    anim->duration = Scale6(frame->duration * anim->scale);
    if (anim->duration == 0)
        anim->duration = 1;
    anim->flags = flags;
    anim->frames = frames;
    anim->mode = mode;
    if (flags & 1)
        return;
    if (flags & 2)
        anim->frame = anim->frameCount - 1;
    else
        anim->frame = 0;
    anim->unk6 = 0;
    anim->timer = 0;
    anim->status = 0;
    anim->scale = 0x40;
}
