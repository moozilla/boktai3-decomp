#include "sprite_effect_playback.h"
void Heap_Free(void *);
void SpriteEffectController_TickAnimation(struct SpriteEffectControllerPool *unusedPool,
                                          struct SpriteEffectController *controller)
{
    struct ContactPosition *position;
    u16 *flags;
    u32 ended, mode;
    if (controller->enter) {
        if (controller->flags & 8)
            ((struct SpriteEffectSequenceDrawable *)controller->drawable)->flags &= ~1;
        else
            ((struct SpriteEffectTileDrawable *)controller->drawable)->flags &= ~1;
        controller->frame = 0;
        controller->enter = 0;
    }
    {
        u16 *lowFlags = &controller->flags;
        mode = *lowFlags & 8;
        flags = lowFlags;
    }
    if (mode) {
        ended = SpriteEffectController_StepDrawable(controller, 1);
        position = &((struct SpriteEffectSequenceDrawable *)controller->drawable)->position;
    } else {
        ended = SpriteEffectController_StepDrawable(controller, 0);
        position = &((struct SpriteEffectTileDrawable *)controller->drawable)->position;
    }
    SpriteEffectController_RefreshCollider(controller, position);
    if (*flags & 0x80) {
        struct ContactPosition *velocity = &controller->velocity;
        position->x = velocity->x + position->x;
        position->y = velocity->y + position->y;
        position->z = velocity->z + position->z;
    }
    controller->frame++;
    if (ended) {
        if (controller->lifetime)
            goto tickTimer;
        if (*flags & 1) {
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
            controller->state = 4;
        controller->enter = 1;
    } else if (controller->lifetime) {
    tickTimer:
        controller->lifetime--;
    }
}
