#include "global.h"
struct SwordChild {
    u8 pad0[0x29];
    u8 flags;
    u8 pad2A[4];
    u8 active, entered;
    u8 pad30[0x74 - 0x30];
    void (*update)(void);
    u8 pad78[0x194 - 0x78];
};
struct SwordGroup {
    u8 pad0[0x5AC];
    struct SwordChild children[3];
    u8 padA68[0xB4C - 0xA68];
    u16 launchDelay, delay;
    u8 padB50[0xCE4 - 0xB50];
    void (*callback)(struct SwordGroup *);
    u8 padCE8[0xD2C - 0xCE8];
    u8 count;
};
void Sword_BeginRestOrbit(void);
void ChildGroup_WaitForInactiveMembers(struct SwordGroup *);
// confidence=low: launches the next resting sword every launchDelay frames; once all are out,
// waits for them to finish.
#ifdef NONMATCHING
// fuzzy: 63.9%
// fix.py best (sweep after the inlined-insn fix)
// agbcc strength-reduces i*0x194 into a pointer; the ROM keeps i and multiplies in the found block.
void sub_080F3420(struct SwordGroup *p)
{
    s32 i;
    if (p->delay) {
        p->delay--;
        return;
    }
    for (i = 0; i < p->count; i++) {
        if (p->children[i].active == 0) {
            struct SwordChild *child = &p->children[i];
            {
                typeof(child->active) *q1 = &child->active;
                s16 z1 = 0;
                *q1 = 1;
                child->entered = z1;
                child->flags |= 1;
                p->children[i].update = Sword_BeginRestOrbit;
                p->delay = p->launchDelay;
                return;
            }
        }
    }

    p->callback = ChildGroup_WaitForInactiveMembers;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080F3420);
#endif
