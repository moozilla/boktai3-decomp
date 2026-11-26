#include "engine/shared/scene_input_widget.h"

s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);

static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}

s32 Anim_Step(struct SceneInputWidget *anim, void *data)
{
    s32 done;
    u32 flags;
    struct AnimationKeyframe *f;

    if (!anim->mode)
        return 0;
    done = 0;
    anim->status = 0;
    if (anim->elapsed == 0) {
        f = &anim->frames[anim->frameIndex];
        Anim_LoadKeyframe(anim, data, f->key, anim->mode);
    }
    anim->value6++;
    if (++anim->elapsed >= anim->duration) {
        if (anim->playbackFlags & 2) {
            if (anim->frameIndex == 0) {
                done = 1;
                goto finished;
            }
            anim->frameIndex--;
        } else {
            anim->frameIndex++;
            if (anim->frameIndex >= anim->frameCount) {
                done = 1;
                anim->frameIndex = anim->frameCount - 1;
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
                anim->flags |= 1;
                goto reset;
            }
            if ((u16)(anim->playbackFlags & 2))
                anim->frameIndex = anim->frameCount - 1;
            else
                anim->frameIndex = 0;
        }
        anim->status |= 4;
    reset:
        f = &anim->frames[anim->frameIndex];
        anim->elapsed = 0;
        anim->duration = Scale6(f->duration * anim->durationScale);
        if (anim->duration == 0)
            anim->duration = 1;
    } else if (anim->elapsed + 1 >= anim->duration) {
        if (anim->playbackFlags & 2) {
            if (anim->frameIndex == 0)
                anim->status |= 1;
        } else if (anim->frameIndex + 1 >= anim->frameCount)
            anim->status |= 1;
    }
    return done;
}
