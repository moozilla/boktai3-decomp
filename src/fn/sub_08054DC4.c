// MATCHING_CONCESSION: register-pinning
#include "display_track.h"
void sub_08220D20(void *, const void *, u32, u32, u32);
void sub_08220D44(void *, u32);
void DisplayTrack_SetSequenceFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track && Script_SeekToKeyword('m')) {
        track->sequenceId = Script_GetValue();
        if (Script_SeekToKeyword('d')) {
            track->sequenceRow = Script_GetValue();
            if (Script_SeekToKeyword('r')) {
                register u32 variant asm("r4");
                u32 event;
                void *state;
                track->mirror = Script_GetValue();
                variant = 0;
                if (Script_SeekToKeyword('E'))
                    variant = Script_GetValue();
                if (variant) {
                    state = &track->sequenceState;
                    sub_08220D20(state, track->sequenceResource, track->sequenceId,
                                 track->sequenceRow, track->mirror);
                } else {
                    state = &track->sequenceState;
                    Anim_SetSequence(state, (void *)track->sequenceResource, track->sequenceId,
                                     track->sequenceRow, track->mirror);
                }
                if (Script_SeekToKeyword('P'))
                    sub_08220D44(state, (u16)Script_GetValue());
                event = Script_SeekToKeyword('o');
                if (event)
                    track->sequenceEvent = Script_GetValue();
                else
                    track->sequenceEvent = event;
                event = track->sequenceEvent;
                if (event && (event = Script_SeekToKeyword('e')))
                    track->sequenceEventValue = Script_GetValue();
                else
                    track->sequenceEventValue = event;
                track->flags |= 8;
            }
        }
    }
}
