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
void ChildGroup_LaunchNextInactiveMember(struct SwordGroup *p)
{
    s32 i;
    u32 stride;
    if (p->delay) {
        p->delay--;
        return;
    }
    i = 0;
    if (i < p->count) {
        stride = sizeof(struct SwordChild);
        for (; i < p->count; i++) {
            if (p->children[i].active == 0) {
                u32 address = i * stride;
                struct SwordChild *child;
                void (**callback)(void);
                u32 offset;
                address += (u32)p;
                address += (u32) & ((struct SwordGroup *)0)->children;
                child = (struct SwordChild *)address;
                {
                    u8 one = 1;
                    u8 *active = &child->active;
                    s16 entered = 0;
                    *active = one;
                    child->entered = entered;
                    child->flags = one | child->flags;
                    offset = i * stride;
                    callback = &p->children[0].update;
                    callback = (void *)((u8 *)callback + offset);
                    *callback = Sword_BeginRestOrbit;
                    p->delay = p->launchDelay;
                    return;
                }
            }
        }
    }

    p->callback = ChildGroup_WaitForInactiveMembers;
}
