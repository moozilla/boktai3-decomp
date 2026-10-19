#include "global.h"
#define ActorAction_RestoreSavedBehavior sub_0819E344
struct ActorActionSequence {
    u8 unknown0[0xA1];
    u8 flags;
    u8 unknownA2[8];
    u8 animation, nextAnimation, mode, submode, previousMode, animationDone, entered;
    u8 unknownB1[0x100 - 0xB1];
    void (*callback)(void);
};
struct ActorActionOwner {
    struct ActorActionSequence actor;
    u8 unknown104[0x384 - 0x104];
    void (*savedCallback)(void);
    u8 savedMode, savedAnimation;
};
static inline u8 ConsumeEntry(struct ActorActionSequence *p)
{
    if (p->entered) {
        p->entered = 0;
        p->animationDone = 0;
        return 1;
    }
    return 0;
}
void sub_0819E344(struct ActorActionOwner *owner)
{
    struct ActorActionSequence *p = &owner->actor;
    ConsumeEntry(p);
    if (p->animationDone) {
        u32 mode = owner->savedMode;
        u32 zero = 0;
        u32 animation;
        void (*callback)(void);
        p->mode = zero;
        p->submode = zero;
        p->previousMode = mode;
        animation = owner->savedAnimation;
        callback = owner->savedCallback;
        p->nextAnimation = animation;
        p->callback = callback;
        p->flags |= 1;
    }
}
