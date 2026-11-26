#include "engine/shared/scene_input_widget.h"
static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
s32 Anim_SetFrameDurationScale(struct SceneInputWidget *anim, u16 scale)
{
    struct AnimationKeyframe *frame = &anim->frames[anim->frameIndex];
    anim->durationScale = scale;
    anim->duration = Scale6(frame->duration * anim->durationScale);
    if (anim->duration == 0)
        anim->duration = 1;
    {
        anim->status &= 0xfe;
        if (anim->elapsed + 1 >= anim->duration) {
            if (anim->playbackFlags & 2) {
                if (anim->frameIndex == 0)
                    anim->status |= 1;
            } else if (anim->frameIndex + 1 >= anim->frameCount)
                anim->status |= 1;
        }
        return 0;
    }
    return 0;
}
