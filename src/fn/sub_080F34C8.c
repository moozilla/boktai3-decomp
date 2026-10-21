// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct ChildGroupMember {
    u8 active;
    u8 pad[0x193];
} __attribute__((packed));
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags8D;
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
void ChildGroup_BeginInactiveState(struct ChildGroup *);
void ChildGroup_WaitForInactiveMembers(struct ChildGroup *p)
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
        register u16 one asm("r0") = 1;
        u8 *q;
        q = &p->state92;
        *q = one;
        q++;
        *q = one;
        p->flags8D = one | p->flags8D;
        p->callbackCC8 = ChildGroup_BeginInactiveState;
    } else if (active < p->count) {
        if (++p->timer <= 7)
            p->callbackCE4 = ChildGroup_WaitForInactiveMembers;
    }
}
