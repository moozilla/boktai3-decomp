#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.9%
// the two zero stores load their constants into swapped registers
typedef void (*ActorUpdate)(u8 *);
typedef void (*ActorStateCallback)(u8 *, u8 *, u32);
/* Per-frame actor tick: clear the display flags, advance the frame counter,
 * run the update hook, then the current state's callback with the old count. */
s32 sub_081CC64C(u8 *a, u8 *b)
{
    u32 frame;
    ActorStateCallback cb;
    u8 zero8 = 0;
    *(u16 *)(b + 0xe) = 0;
    b[7] = zero8;
    frame = (*(u32 *)(b + 0x10))++;
    (*(ActorUpdate *)(b + 0x2e4))(b);
    cb = *(ActorStateCallback *)(b + 0x2cc);
    if (cb)
        cb(a, b, frame);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081CC64C);
#endif
