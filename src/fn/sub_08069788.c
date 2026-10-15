// MATCHING_CONCESSION: register-pinning
#include "bouncing_object.h"
#define BouncingObject_IsActive sub_08069788
u32 BouncingObject_IsActive(struct BouncingObject *input)
{
    struct BouncingObject *slot = input;
    register struct BouncingObject *late asm("r2") = slot;
    if (slot) {
        register u32 indexOffset asm("r3") = (u32) & ((struct BouncingObject *)0)->index;
        register u8 *location asm("r0");
        u32 index;
        asm("" : "+r"(indexOffset));
        location = (void *)((u32)slot + indexOffset);
        index = *location;
        index = (s8)index;
        if ((s32)index >= 0) {
            u32 offset = (u32) & ((struct BouncingObject *)0)->timer;
            u16 *timer;
            timer = (void *)((u32)slot + offset);
            if (*timer) {
                u32 stateOffset = (u32) & ((struct BouncingObject *)0)->state;
                register u8 *state asm("r0");
                state = (void *)((u32)late + stateOffset);
                if (*state != 2)
                    return 1;
            }
        }
    }
    return 0;
}
