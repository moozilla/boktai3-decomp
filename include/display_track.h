#ifndef GUARD_DISPLAY_TRACK_H
#define GUARD_DISPLAY_TRACK_H
#include "contact_swarm.h"
#include "display_sequence_inline.h"
struct DisplayTrackGraphics {
    u8 unknown0[6];
    u16 defaultPalette;
    u8 unknown8[4];
    const void *palette;
    u8 unknown10[0xC];
};
struct DisplayTrack {
    u16 tag;
    u8 flags, mirror, runDuringPause;
    u8 unknown5[3];
    u16 blendedPalette[16];
    struct ContactSwarmSprite sprite;
    struct DisplayTrackGraphics graphics;
    struct ContactPosition origin, target;
    u16 moveDuration, moveFrame, paletteBase, paletteStart, paletteEnd, paletteFrame,
        paletteDuration, unknown8E;
    u16 scaleStart, scaleEnd, scaleFrame, scaleDuration;
    u8 sequenceId, sequenceRow, sequenceEvent, unknown9B;
    struct DisplaySequenceState sequenceState;
    const void *sequenceResource;
    u32 sequenceEventValue;
    void *ownedResource;
};
struct DisplayTrackManager {
    u8 task[0x18];
    u8 capacity, count, unknown1A[2];
    struct DisplayTrack entries[1];
};
/* The track's sequence step: the first flip goes through the track's own sprite. */
static inline u32 DisplayTrack_StepSequence(struct DisplayTrack *track,
                                            struct ContactSwarmSprite *sprite,
                                            struct DisplaySequenceState *state)
{
    const u16 *command = &state->commands[state->index];
    u32 ended;
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            track->sprite.flags |= 4;
        else
            track->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

/* 0x02000494 */
extern struct DisplayTrackManager *gDisplayTrackManager;
struct DisplayTrack *DisplayTrack_FindFromScript(void);
void DisplayTrack_Update(struct DisplayTrack *);
void DisplayTrack_AddFromScript(void);
u32 DisplayTrack_TickManager(struct DisplayTrackManager *);
s32 DisplayTrack_DestroyManager(struct DisplayTrackManager *);
s32 DisplayTrack_InitManager(struct DisplayTrackManager *, u32, u32, u32);
struct DisplayTrackManager *DisplayTrack_CreateManagerFromScript(u32, u32);
#endif
