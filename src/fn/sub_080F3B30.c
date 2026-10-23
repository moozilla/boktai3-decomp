#include "global.h"
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0xC3A];
    void (*callbackCC8)(struct ChildGroup *);
    u8 padCCC[0xB4];
    s32 elapsedD80;
};
void ChildGroup_UpdateTrailingMembers(struct ChildGroup *);

/* After 30 frames, hand over to ChildGroup_UpdateTrailingMembers and set
   flag bit 0. The bit is a local because the ROM loads it before the
   flag's address. */
void sub_080F3B30(struct ChildGroup *p)
{
    if (p->elapsedD80 > 30) {
        p->callbackCC8 = ChildGroup_UpdateTrailingMembers;
        {
            u8 bit = 1;
            p->flags |= bit;
        }
    }
}
