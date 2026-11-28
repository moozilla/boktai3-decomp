#include "engine/video/display_sequence_inline.h"

struct SequenceEntry {
    u8 unk0, frameCount;
    u16 offset;
};

u32 Anim_SetSequence(struct DisplaySequenceState *state, u8 *data, u16 sequence, u8 row, u8 flags)
{
    struct SequenceEntry *entry = (struct SequenceEntry *)(data + (sequence * 4 + 4));
    /* Reused for the entry's frame list and then the first command. */
    u8 *address = data + entry->offset;
    u32 rowStart = entry->frameCount * row * 2;

    state->commands = (const u16 *)(address + rowStart);
    state->speed = 0x40;
    if (state->sequenceId != sequence || !state->frameCount ||
        ((flags & 4) && !(state->flags & 4)) || (!(flags & 4) && (state->flags & 4))) {
        state->frameCount = entry->frameCount;
        if (flags & 4)
            state->index = state->frameCount - 1;
        else
            state->index = 0;
        address = (u8 *)((u32)state->commands + (state->index << 1));
        {
            u32 command = DisplaySequence_ReadCommand((const u16 *)address);
            u32 weight = 0xF;
            weight &= command;
            state->frameWeight = weight;
            state->duration = weight;
        }
        state->elapsed = 0;
        state->sequenceId = sequence;
        state->flags = flags;
        return 1;
    }
    state->flags = flags;
    return 0;
}
