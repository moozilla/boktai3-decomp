#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.8%
// foundry draft (worker best 90.8%); what differs is mostly register allocation and instruction order
#define ChildGroup_WaitForInactiveMembers sub_080F34C8
#define ChildGroup_BeginInactiveState     sub_080F3ABC

struct ChildGroupMember {
    u8 active;
    u8 pad[0x193];
} __attribute__((packed));
struct ChildGroup {
    u8 pad0[0x8D];
    u8 inactive : 1;
    u8 flags : 7;
    u8 pad8E[4];
    u8 state92, state93;
    u8 pad94[0x546];
    struct ChildGroupMember members[4];
    u8 padC2A[0x9E];
    void (*callbackCC8)(struct ChildGroup *);
    u8 padCCC[0x18];
    void (*callbackCE4)(struct ChildGroup *);
    u8 padCE8[0x40];
    u16 timer;
    u8 padD2A[2];
    u8 count;
};
void sub_080F3ABC(struct ChildGroup *);
void sub_080F34C8(struct ChildGroup *p)
{
    s32 active = 0;
    s32 n;
    struct ChildGroupMember *member;
    if (active < p->count) {
        member = p->members;
        {
            u32 stride = sizeof(struct ChildGroupMember);
            n = p->count;
            do {
                if (member->active)
                    active++;
                member = (struct ChildGroupMember *)((u8 *)member + stride);
            } while (--n != 0);
        }
    }
    if (active == 0) {
        u16 one = 1;
        p->state92 = one;
        p->state93 = one;
        p->inactive = 1;
        p->callbackCC8 = sub_080F3ABC;
    } else if (active < p->count) {
        if (++p->timer <= 7)
            p->callbackCE4 = sub_080F34C8;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F34C8);
#endif
