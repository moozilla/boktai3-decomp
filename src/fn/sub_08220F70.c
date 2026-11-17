#include "scene_input_widget.h"

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

    if (!anim->attribute1B)
        return 0;
    done = 0;
    anim->status = 0;
    if (anim->fieldC == 0) {
        f = &anim->frames[anim->frameTimer];
        Anim_LoadKeyframe(anim, data, f->key, anim->attribute1B);
    }
    anim->value6++;
    if (++anim->fieldC >= anim->attributeE) {
        if (anim->value18 & 2) {
            if (anim->frameTimer == 0) {
                done = 1;
                goto finished;
            }
            anim->frameTimer--;
        } else {
            anim->frameTimer++;
            if (anim->frameTimer >= anim->frameDuration) {
                done = 1;
                anim->frameTimer = anim->frameDuration - 1;
            }
        }
        if (done) {
        finished:
            anim->status |= 2;
            if (anim->attribute1B == 2) {
                anim->attribute1B = 0;
                goto reset;
            }
            if (anim->attribute1B == 3) {
                anim->attribute1B = 0;
                anim->flags |= 1;
                goto reset;
            }
            if ((u16)(anim->value18 & 2))
                anim->frameTimer = anim->frameDuration - 1;
            else
                anim->frameTimer = 0;
        }
        anim->status |= 4;
    reset:
        f = &anim->frames[anim->frameTimer];
        anim->fieldC = 0;
        anim->attributeE = Scale6(f->duration * anim->scale);
        if (anim->attributeE == 0)
            anim->attributeE = 1;
    } else if (anim->fieldC + 1 >= anim->attributeE) {
        if (anim->value18 & 2) {
            if (anim->frameTimer == 0)
                anim->status |= 1;
        } else if (anim->frameTimer + 1 >= anim->frameDuration)
            anim->status |= 1;
    }
    return done;
}
