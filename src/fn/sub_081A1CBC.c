#include "global.h"
#define ActorReaction_BeginCollisionPhase sub_081A1CBC
#define ActorCollision_ConfigurePair      sub_0819B96C
struct ActorReaction {
    u8 unknown0[0x9C];
    u32 flags;
    u8 unknownA0, flagsA1;
    u8 unknownA2[0xF];
    u8 pending;
    u8 unknownB2[0xC2];
    u8 collider174[0x20];
    u8 collider194[0x20];
    u8 unknown1B4[0x15C];
    u16 state;
};
void sub_0819B96C(struct ActorReaction *, void *, void *, u32, u32, u32);
void sub_081A1CBC(struct ActorReaction *actor)
{
    u32 flags = 0x20000C;
    actor->flags |= flags;
    actor->flags |= 0x40;
    actor->flagsA1 |= 4;
    if (actor->pending) {
        actor->pending = 0;
        ActorCollision_ConfigurePair(actor, actor->collider194, actor->collider174, 15, 1, 0);
        actor->state = 6;
    }
}
