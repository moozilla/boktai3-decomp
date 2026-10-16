// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_controller.h"
static inline u8 SpriteEffectController_IsUsed(struct SpriteEffectControllerPool *pool, s32 index)
{
    if (pool->used & (1 << index))
        return TRUE;
    return FALSE;
}
void SpriteEffectController_QueueColliders(struct SpriteEffectControllerPool *input)
{
    register struct SpriteEffectControllerPool *pool asm("r6") = input;
    struct SpriteEffectController *first = pool->slots;
    register s32 scheduled asm("r8");
    register s32 i asm("r5");
    register u8 *readDelay asm("r3");
    register u8 *writeDelay asm("r4");
    struct SpriteEffectController *controller;
    u32 temporary = 0;
    asm("" : "+r"(temporary) : : "r0", "r1", "r2", "r3", "r4", "r5");
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
                    register u32 flags asm("r1") = *flagsAddress;
                    u32 value;
                    temporary = 0x100;
                    value = temporary | flags;
                    *flagsAddress = value;
                    asm("" : : "r"(flags), "r"(temporary));
                }
                {
                    register s32 one asm("r0") = 1;
                    asm("" : "+r"(one));
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
