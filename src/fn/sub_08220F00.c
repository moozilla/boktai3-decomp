#include "scene_input_widget.h"
static inline s32 Scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
s32 Anim_SetFrameDurationScale(struct SceneInputWidget *anim, u16 scale)
{
    struct AnimationKeyframe *frame = &anim->frames[anim->frameTimer];
    anim->scale = scale;
    anim->attributeE = Scale6(frame->duration * anim->scale);
    if (anim->attributeE == 0)
        anim->attributeE = 1;
    {
        anim->status &= 0xfe;
        if (anim->fieldC + 1 >= anim->attributeE) {
            if (anim->value18 & 2) {
                if (anim->frameTimer == 0)
                    anim->status |= 1;
            } else if (anim->frameTimer + 1 >= anim->frameDuration)
                anim->status |= 1;
        }
        return 0;
    }
    return 0;
}
