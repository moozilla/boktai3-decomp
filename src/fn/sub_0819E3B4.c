#include "global.h"

// confidence=low: enters animation 38 and notifies the linked actor once.
#define ActorAction_BeginLinkedAnimation sub_0819E3B4
struct LinkedAction {
    u8 unknown0[0xAA];
    u8 animation, previousAnimation;
    u8 unknownAC[3];
    u8 animationDone, entered;
    u8 unknownB1[0xC4 - 0xB1];
    u32 timer;
    u8 unknownC8[0x7E4 - 0xC8];
    void *linked;
};
void sub_08159164(void *);
static inline u8 TakeFlag(struct LinkedAction *p)
{
    if (p->entered) {
        p->entered = 0;
        p->animationDone = 0;
        return TRUE;
    }
    return FALSE;
}
void ActorAction_BeginLinkedAnimation(struct LinkedAction *arg)
{
    struct LinkedAction *p = arg;
    struct LinkedAction *base = p;
    if (TakeFlag(p)) {
        if (p->previousAnimation != 38) {
            u32 animation = 38;
            p->animation = animation;
        }
        sub_08159164(base->linked);
    }
    p->timer++;
}
