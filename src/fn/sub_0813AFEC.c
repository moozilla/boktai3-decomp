#include "global.h"
struct ActorSequence {
    u8 pad00[0x457];
    u8 sequence;
    u8 variant;
    u8 pad459;
    u16 timer;
    u8 active;
};
void ActorSequence_Begin(struct ActorSequence *actor, u32 sequence, u32 variant)
{
    actor->active = 1;
    actor->sequence = sequence;
    actor->variant = variant;
    actor->timer = 0;
}
