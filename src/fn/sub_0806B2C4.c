#include "sprite_effect_playback.h"
u32 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void Heap_Free(void *);
void SpriteEffectController_FadeAndDestroy(struct SpriteEffectControllerPool *unused,
                                           struct SpriteEffectController *controller)
{
    u16 *flags;
    u32 mode;
    if (controller->enter) {
        controller->enter = 0;
        controller->frame = 0;
    }
    {
        u16 *lowFlags = &controller->flags;
        mode = *lowFlags & 8;
        flags = lowFlags;
    }
    if (mode)
        SpriteEffectController_StepDrawable(controller, 1);
    else
        SpriteEffectController_StepDrawable(controller, 0);
    {
        void *blink;
        if (*flags & 8)
            blink = (struct SpriteEffectSequenceDrawable *)controller->drawable;
        else
            blink = (struct SpriteEffectTileDrawable *)controller->drawable;
        mode = Sprite_BlinkUntilExpired(blink, controller->frame, 60, 40);
    }
    if ((u8)mode) {
        struct SpriteEffectDrawablePrefix *drawable;
        gSpriteEffectControllerPool->used &= ~(1 << controller->index);
        controller->state = 0;
        mode = *flags & 8;
        if (mode) {
            drawable = controller->drawable;
            if (drawable->active) {
                Sprite_Remove(drawable);
                Heap_Free(controller->drawable);
            }
        } else {
            drawable = controller->drawable;
            if (drawable->active) {
                Effect_Remove(drawable);
                Heap_Free(controller->drawable);
            }
        }
        controller->index = -1;
        *flags = 0;
    } else
        controller->frame++;
}
