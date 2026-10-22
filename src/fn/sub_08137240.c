#include "global.h"

s32 Callback_InvokeActor(void *, void *);

// confidence=low: scene actor state tick: clears the entry flag, runs the update callback and counts frames while it reports idle.
void sub_08137240(u8 *actor)
{
    if (actor[0x2b1])
        actor[0x2b1] = 0;
    if (!(u8)Callback_InvokeActor(actor, *(void **)(actor + 0x270)))
        (*(s32 *)(actor + 0x2ac))++;
}
