#include "sprite_effect_controller.h"
static inline u8 SpriteEffectController_IsUsed(struct SpriteEffectControllerPool *pool, s32 index)
{
    if (pool->used & (1 << index))
        return TRUE;
    return FALSE;
}
void SpriteEffectController_QueueColliders(struct SpriteEffectControllerPool *input)
{
    struct SpriteEffectControllerPool *pool = input;
    struct SpriteEffectController *first = pool->slots;
    s32 scheduled;
    s32 i;
    u8 *readDelay;
    u8 *writeDelay;
    struct SpriteEffectController *controller;
    u32 temporary = 0;
    scheduled = temporary;
    i = 0;
    readDelay = &pool->slots[0].colliderDelay;
    writeDelay = &pool->slots[0].colliderDelay;
    controller = first;
    do {
        if (SpriteEffectController_IsUsed(pool, i) && (first->flags & 4)) {
            u32 delay = *readDelay;
            if (delay)
                *readDelay = delay - 1;
            else if (scheduled <= 5) {
                *writeDelay = 3;
                {
                    u16 *flagsAddress = &controller->flags;
                    u32 flags = *flagsAddress;
                    u32 value;
                    temporary = 0x100;
                    value = temporary | flags;
                    *flagsAddress = value;
                }
                {
                    s32 one = 1;
                    scheduled += one;
                }
            }
        }
        readDelay += sizeof(*controller);
        writeDelay += sizeof(*controller);
        temporary = sizeof(*controller);
        controller = (struct SpriteEffectController *)((u8 *)controller + temporary);
        i++;
    } while (i <= 23);
}
