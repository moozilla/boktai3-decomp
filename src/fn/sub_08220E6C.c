#include "global.h"
struct AnimFrame {
    u16 key, duration;
};
struct AnimSequence {
    u16 unk0, count;
    u32 offset;
};
struct AnimData {
    u8 pad0[4];
    u16 sequenceCount;
    u8 pad6[10];
    struct AnimSequence *sequences;
    u8 pad14[4];
    u8 *frames;
};
struct AnimState {
    u8 pad0[6];
    u16 unk6;
    u8 pad8[4];
    u16 timer, duration, scale, sequence, frame, frameCount, flags;
    u8 pad1a;
    u8 param;
    u8 pad1c;
    u8 finished;
    u8 pad1e[0x54 - 0x1e];
    struct AnimFrame *frames;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
void sub_08220E6C(struct AnimState *anim, struct AnimData *data, u16 sequence, u16 index)
{
    struct AnimSequence *table = data->sequences;
    struct AnimSequence *entry = (struct AnimSequence *)((sequence * 8) + (u32)table);
    struct AnimFrame *frames = (struct AnimFrame *)(data->frames + entry->offset);
    struct AnimFrame *frame;
    anim->frame = index;
    if (index >= entry->count)
        anim->frame = entry->count - 1;
    frame = &frames[anim->frame];
    Anim_LoadKeyframe(anim, data, frame->key, anim->param);
    anim->frameCount = entry->count;
    anim->duration = Scale6(frame->duration * anim->scale);
    if (anim->duration == 0)
        anim->duration = 1;
    anim->timer = 0;
    anim->unk6 = 0;
    anim->finished = 0;
    if (anim->timer + 1 >= anim->duration) {
        if (anim->flags & 2) {
            if (anim->frame != 0)
                return;
        } else if (anim->frame + 1 < anim->frameCount)
            return;
        anim->finished = 1;
    }
}
