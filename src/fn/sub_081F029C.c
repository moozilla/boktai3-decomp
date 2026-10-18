/* Tentative family and field names, based on static accesses. */
#define ActorReaction_MarkSequenceHit sub_081F029C
#include "global.h"
void sub_081EE518(void);
struct ActorReaction {
    u8 unknown0[0x100];
    void (*update)(void);
    u8 unknown104[0x17C];
    s32 speed;
    u8 unknown284[0x11C];
    u32 sequence;
    u8 unknown3A4[0x1E8];
    u32 hit;
    u32 sequenceHit;
};
struct ActorReactionEvent {
    u32 unknown0, sequence;
};
void sub_081F029C(struct ActorReaction *p, struct ActorReactionEvent *event)
{
    p->hit = 1;
    if (p->update == sub_081EE518 && event->sequence == p->sequence + 1) {
        p->sequenceHit = 1;
        p->speed >>= 1;
    }
}
