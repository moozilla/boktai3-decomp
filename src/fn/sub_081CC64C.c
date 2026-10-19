#include "global.h"
/* The return value is set up as a copy of a zero before the stores (found by tools/permute.py):
 * it puts the halfword zero in r0 and the byte zero in r1, as in the ROM. */
#define Actor_TickCallbacks sub_081CC64C
typedef void (*ActorUpdate)(u8 *);
typedef void (*ActorStateCallback)(u8 *, u8 *, u32);
/* Per-frame actor tick: clear the display flags, advance the frame counter,
 * run the update hook, then the current state's callback with the old count. */
s32 Actor_TickCallbacks(u8 *a, u8 *b)
{
    s16 result;
    u8 zero8 = 0;
    u32 frame;
    ActorStateCallback cb;
    s16 zero = 0;
    result = zero;

    *(u16 *)(b + 0xe) = 0;
    b[7] = zero8;
    frame = (*(u32 *)(b + 0x10))++;
    (*(ActorUpdate *)(b + 0x2e4))(b);
    cb = *(ActorStateCallback *)(b + 0x2cc);
    if (cb)
        cb(a, b, frame);
    return result;
}
