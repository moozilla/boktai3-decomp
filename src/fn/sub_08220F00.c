#include "global.h"
struct AnimFrame {
    u16 key, duration;
};
struct AnimState {
    u8 pad0[0xc];
    u16 timer, duration, scale, sequence, frame, frameCount, flags;
    u8 pad1a[3];
    u8 status;
    u8 pad1e[0x54 - 0x1e];
    struct AnimFrame *frames;
};
static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
s32 Anim_SetFrameDurationScale(struct AnimState *anim, u16 scale)
{
    struct AnimFrame *frame = &anim->frames[anim->frame];
    anim->scale = scale;
    anim->duration = Scale6(frame->duration * anim->scale);
    if (anim->duration == 0)
        anim->duration = 1;
    {
        s32 zero = 0;
        anim->status &= 0xfe;
        if (anim->timer + 1 >= anim->duration) {
            if (anim->flags & 2) {
                if (anim->frame == zero)
                    anim->status |= 1;
            } else if (anim->frame + 1 >= anim->frameCount)
                anim->status |= 1;
        }
        return zero;
    }
    return 0;
}
