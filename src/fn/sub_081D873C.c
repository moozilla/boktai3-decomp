#include "player_cue_sequence.h"
struct TaskSequenceConfig {
    u16 flags, duration;
};
struct Actor {
    u32 tick, flags;
    u8 pad8[0x10];
    struct TaskSequenceConfig config;
    u8 pad1c[0x8c - 0x1c];
    struct ContactSwarmSprite render;
    u8 unknownB8[0x180 - 0xB8];
    struct DisplaySequenceState frames;
};
s32 Task4E01_StepTimedSequence(struct Actor *p)
{
    struct TaskSequenceConfig *config = &p->config;
    u32 bit = 2;
    p->flags |= bit;
    PlayerCue_StepSequence(&p->render, &p->frames);
    if (p->tick >= config->duration)
        return 2;
    return 0;
}
