#include "display_track.h"
void DisplayTrack_SetScaleTrackFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track && Script_SeekToKeyword('s')) {
        track->scaleStart = Script_GetValue();
        if (Script_SeekToKeyword('e')) {
            track->scaleEnd = Script_GetValue();
            if (Script_SeekToKeyword('f')) {
                track->scaleDuration = Script_GetValue();
                {
                    u32 end = track->scaleEnd;
                    track->sprite.unknown8[0] = end;
                    track->sprite.unknown8[1] = end;
                }
                if (track->scaleDuration <= 1) {
                    if ((s8)track->sprite.unknown8[0] != 64)
                        track->sprite.flags |= 2;
                } else {
                    track->sprite.flags |= 2;
                    track->scaleFrame = 0;
                    track->flags |= 4;
                }
            }
        }
    }
}
