// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.1%
// Open: reload of the hoisted flag-mask (sb) after the calls uses r1, ROM uses r2.
struct Node {
    struct Node *next;
    u16 id;
    u16 flags;
    u16 pad8;
    u16 mask;
    u8 pad1[0x34 - 0xc];
    u32 f34;
    u8 pad2[0x3e - 0x38];
    u16 f3e;
    u8 pad3[0x4c - 0x40];
    u32 cb;
    u32 arg;
};
void PositionList_UpdateTranslatedCoordinates(void *);
s32 Box_Overlaps(void *, void *);
s32 sub_08249240(struct Node *, struct Node *, s32 (*)(void *, void *));
void sub_08249244(struct Node *, struct Node *, u32, u32);
void Collision_ProcessPairs(struct Node **listA, struct Node **listB)
{
    struct Node *a;
    struct Node *b;
    struct Node *cur;
    struct Node *other;
    int new_var;
    register u32 two asm("sb");
    PositionList_UpdateTranslatedCoordinates(listA);
    cur = *listA;
    while (cur != 0) {
        a = cur;
        cur = a->next;
        b = *listB;
        if (b != 0) {
            {
                register u32 t asm("r2") = 2;
                asm("" : "+r"(t));
                two = t;
            }
            asm("" : "+r"(two));
            do {

                other = b;
                b = other->next;
                if ((a->mask & other->mask) != 0)
                    continue;
                if ((other->flags & 4) != 0)
                    continue;
                if (other->id != 0 && other->id == a->id)
                    continue;
                if ((a->flags & 1) == 0 && (other->flags & 1) != 0)
                    continue;
                if ((a->flags & 0x1000) != 0 && (other->flags & 0x1000) == 0)
                    continue;
                if (sub_08249240(a, other, Box_Overlaps) == 0)
                    continue;
                other->f3e = 0;
                new_var = 0x200000;
                if ((a->f34 & new_var) != 0) {
                    if ((a->flags & two) == 0 && other->cb != 0)
                        sub_08249244(a, other, other->arg, other->cb);
                    if ((other->flags & two) == 0 && a->cb != 0)
                        sub_08249244(a, other, a->arg, a->cb);
                } else {
                    if ((other->flags & two) == 0 && a->cb != 0)
                        sub_08249244(a, other, a->arg, a->cb);
                    if ((a->flags & two) == 0 && other->cb != 0)
                        sub_08249244(a, other, other->arg, other->cb);
                }
            } while (b != 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Collision_ProcessPairs);
#endif
