// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
#define StackedActor_ConsumeTriggers sub_080530EC
extern u8 *gUnk_0200047C;
u32 sub_08020D54(void *, void *);
void sub_08052660(struct StackedActor *);
void sub_0805278C(struct StackedActor *);
void StackedActor_ConsumeTriggers(struct StackedActor *actor)
{
    u8 *receiver = actor->trigger;
    register s32 i asm("r5") = 0;
    u8 **manager = &gUnk_0200047C;
    u8 *counts = (u8 *)actor + 0x1E;
    if (i >= counts[(*manager)[0x24]])
        goto done;
    {
        u8 **cache = manager;
        s32 count;
        do {
            u8 **slot = cache;
            u8 *current = *slot;
            s32 offset = i * 4;
            u8 *base;
            u8 *message;
            asm("" : "+r"(receiver));
            offset += current[0x24] * 16;
            base = receiver + 8;
            base += offset;
            message = *(u8 **)base;
            sub_08020D54(receiver, message);
            switch (message[6]) {
                case 5:
                    sub_08052628(actor, sub_08052660);
                    break;
                case 6:
                    sub_08052628(actor, sub_0805278C);
                    break;
            }
            i++;
            {
                u8 *current = *cache;
                register u8 *counts asm("r0") = receiver + 6;
                u32 selected = current[0x24];
                counts += selected;
                count = *counts;
            }
        } while (i < count);
    }
done:;
}
