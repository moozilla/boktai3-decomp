#include "display_track.h"
void Anim_RestartSequence(void *, const void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void DisplayTrack_SetSequenceFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track && Script_SeekToKeyword('m')) {
        track->sequenceId = Script_GetValue();
        if (Script_SeekToKeyword('d')) {
            track->sequenceRow = Script_GetValue();
            if (Script_SeekToKeyword('r')) {
                u32 variant;
                u32 event;
                void *state;
                track->mirror = Script_GetValue();
                do {
                variant = 0;
                    if (Script_SeekToKeyword('E')) {
                    variant = Script_GetValue();
                    }
                } while (0);
                if (variant) {
                    state = &track->sequenceState;
                    Anim_RestartSequence(state, track->sequenceResource, track->sequenceId,
                                         track->sequenceRow, track->mirror);
                } else {
                    state = &track->sequenceState;
                    Anim_SetSequence(state, (void *)track->sequenceResource, track->sequenceId,
                                     track->sequenceRow, track->mirror);
                }
                if (Script_SeekToKeyword('P')) {
                    Anim_SetDurationScale(state, (u16)Script_GetValue());
                }
                event = Script_SeekToKeyword('o');
                if (event) {
                    track->sequenceEvent = Script_GetValue();
                } else {
                    track->sequenceEvent = event;
                }
                event = track->sequenceEvent;
                if (event && (event = Script_SeekToKeyword('e'))) {
                    track->sequenceEventValue = Script_GetValue();
                } else {
                    track->sequenceEventValue = event;
                }
                track->flags |= 8;
            }
        }
    }
}
