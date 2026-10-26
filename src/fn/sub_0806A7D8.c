// MATCHING_CONCESSION: register-pinning
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
        register u32 loaded asm("r1") = *lowFlags;
        u32 selected = loaded & 8;
        asm("" : "+r"(selected) : "r"(lowFlags));
        flags = lowFlags;
        mode = selected;
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
        register struct ContactPosition *velocity asm("r1") = &controller->velocity;
        register u32 x asm("r0");
        x = (u16)position->x;
        asm("" : "+r"(x));
        position->x = x + (u16)controller->velocity.x;
        x = (u16)position->y;
        asm("" : "+r"(x));
        position->y = x + (u16)velocity->y;
        x = (u16)position->z;
        asm("" : "+r"(x));
        position->z = x + (u16)velocity->z;
    }
    controller->frame++;
    if (ended) {
        if (controller->lifetime)
            goto tickTimer;
        if (*flags & 1) {
            struct SpriteEffectDrawablePrefix *drawable;
            gSpriteEffectControllerPool->used &= ~(1 << controller->index);
            controller->state = 0;
            {
                register u16 *reloadFlags asm("r2") = flags;
                asm("" : "+r"(reloadFlags));
                mode = *reloadFlags & 8;
            }
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
