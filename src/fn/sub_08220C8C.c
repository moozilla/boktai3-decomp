#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.2%
// Open: the command table pointer is built in r1 instead of r0.
struct DisplaySequenceState {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, sequence, speed, elapsed;
};
u32 Anim_SetSequence(struct DisplaySequenceState *state, u8 *data, u16 sequence, u8 row, u8 flags)
{
    u8 *entry = data + (sequence * 4 + 4);
    state->commands = (const u16 *)(entry[1] * row * 2 + (data + *(u16 *)(entry + 2)));
    state->speed = 0x40;
    if (state->sequence == sequence && state->frameCount) {
        if (flags & 4) {
            if (!(state->flags & 4))
                goto restart;
            goto same;
        }
        if (!(state->flags & 4))
            goto same;
    }
restart:
    state->frameCount = entry[1];
    if (flags & 4)
        state->index = state->frameCount - 1;
    else
        state->index = 0;
    {
        u32 offset = state->index << 1;
        const volatile u16 *command;
        u8 weight;
        entry = (u8 *)state->commands;
        command = (const volatile u16 *)(entry + offset);
        weight = *command & 0xF;
        state->frameWeight = weight;
        state->duration = weight;
    }
    state->elapsed = 0;
    state->sequence = sequence;
    state->flags = flags;
    return 1;
same:
    state->flags = flags;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Anim_SetSequence);
#endif
