#include "global.h"

struct AnimFrame {
    u16 key, duration;
};
struct AnimState {
    u8 pad0[6];
    u16 unk6;
    u32 unk8;
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

s32 Anim_Step(struct AnimState *anim, void *data)
{
    s32 done;
    u32 flags;
    struct AnimFrame *f;

    if (!anim->mode)
        return 0;
    done = 0;
    anim->status = 0;
    if (anim->timer == 0) {
        f = &anim->frames[anim->frame];
        Anim_LoadKeyframe(anim, data, f->key, anim->mode);
    }
    anim->unk6++;
    if (++anim->timer >= anim->duration) {
        if (anim->flags & 2) {
            if (anim->frame == 0) {
                done = 1;
                goto finished;
            }
            anim->frame--;
        } else {
            anim->frame++;
            if (anim->frame >= anim->frameCount) {
                done = 1;
                anim->frame = anim->frameCount - 1;
            }
        }
        if (done) {
        finished:
            anim->status |= 2;
            if (anim->mode == 2) {
                anim->mode = 0;
                goto reset;
            }
            if (anim->mode == 3) {
                anim->mode = 0;
                anim->unk8 |= 1;
                goto reset;
            }
            if ((u16)(anim->flags & 2))
                anim->frame = anim->frameCount - 1;
            else
                anim->frame = 0;
        }
        anim->status |= 4;
    reset:
        f = &anim->frames[anim->frame];
        anim->timer = 0;
        anim->duration = Scale6(f->duration * anim->scale);
        if (anim->duration == 0)
            anim->duration = 1;
    } else if (anim->timer + 1 >= anim->duration) {
        if (anim->flags & 2) {
            if (anim->frame == 0)
                anim->status |= 1;
        } else if (anim->frame + 1 >= anim->frameCount)
            anim->status |= 1;
    }
    return done;
}
